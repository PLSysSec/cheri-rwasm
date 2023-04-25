

// definitions needed by rlbox 


typedef WasmModule* (*new_wasm_module_t)(i32 argc, Handle argv);
typedef void (*destroy_wasm_module_t)(WasmModule* ctx);
typedef u32 (*lookup_func_index_t)(WasmModule* ctx, u32 param_count, u32 result_count, Handle types);
typedef u32 (*add_callback_t)(WasmModule* ctx, u32 func_type_idx, Handle func_ptr);
typedef void (*remove_callback_t)(WasmModule* ctx, u32 callback_idx);


WasmModule* new_wasm_module(i32 argc, Handle argv);
void destroy_wasm_module(WasmModule* ctx);
u32 lookup_func_index(WasmModule* ctx, u32 param_count, u32 result_count, Handle types);
u32 add_mswasm_callback(WasmModule* ctx, u32 func_type_idx, Handle func_ptr);
void remove_mswasm_callback(WasmModule* ctx, u32 callback_idx);


typedef struct mswasm_sandbox_funcs_t {
  new_wasm_module_t create_mswasm_sandbox;
  destroy_wasm_module_t destroy_mswasm_sandbox;
  lookup_func_index_t lookup_mswasm_func_index;
  add_callback_t add_mswasm_callback;
  remove_callback_t remove_mswasm_callback;
} mswasm_sandbox_funcs_t;

mswasm_sandbox_funcs_t get_mswasm_sandbox_info() {
  mswasm_sandbox_funcs_t ret;
  //ret.wasm_rt_sys_init = &wasm_rt_sys_init;
  ret.create_mswasm_sandbox = &new_wasm_module; // rwasm naming convention
  ret.destroy_mswasm_sandbox = &destroy_wasm_module; // rwasm naming convention
  // ret.lookup_wasm2c_nonfunc_export = &lookup_wasm2c_nonfunc_export;
  ret.lookup_mswasm_func_index = &lookup_func_index;
  ret.add_mswasm_callback = &add_mswasm_callback;
  ret.remove_mswasm_callback = &remove_mswasm_callback;
  return ret;
}


void remove_mswasm_callback(WasmModule* ctx, uint32_t callback_idx){
  ctx->indirect_call_table[callback_idx].func_ptr = NULL;
}

// linearly scan through callback table for a free entry and replace it
u32 add_mswasm_callback(WasmModule* ctx, u32 func_type_idx, Handle func_ptr){
  for (int idx = 0; idx < 128; idx++){
    if (ctx->indirect_call_table[idx].func_ptr == NULL) {
      ctx->indirect_call_table[idx].type_idx = func_type_idx;
      ctx->indirect_call_table[idx].func_ptr = func_ptr;
    }
  }
  assert(false && "Exceeded max callback slots (128)");
}




// // typedef uint32_t (*lookup_wasm2c_func_index_t)(WasmModule* ctx,
// //                                                uint32_t param_count,
// //                                                uint32_t result_count,
// //                                                wasm_rt_type_t* types);


// typedef struct mswasm_sandbox_funcs_t {
//   new_wasm_module_t new_wasm_module;
//   // destroy_wasm_module_t destroy_wasm_module;
//   // add_callback_t add_mswasm_callback;
//   // remove_callback_t remove_mswasm_callback;
// } mswasm_sandbox_funcs_t;


//}

FuncType mswasm_create_func_type(uint32_t from_count, 
                                 uint32_t to_count, 
                                 mswasm_type_t* types){
      FuncType func_type;
    
    // 1) allocate space for parameters
    func_type.from_count = from_count;
    if (func_type.from_count != 0) {
      func_type.from = malloc(from_count * sizeof(mswasm_type_t));
      assert(func_type.from != 0);
    } else {
      func_type.from = 0;
    }

    // 2) allocate space for result
    func_type.to_count = to_count;
    if (func_type.to_count != 0) {
      func_type.to = malloc(to_count * sizeof(mswasm_type_t));
      assert(func_type.to != 0);
    } else {
      func_type.to = 0;
    }

    // 3) copy in parameters and result
    uint32_t i;
    for (i = 0; i < from_count; ++i)
      func_type.from[i] = types[i];
    for (i = 0; i < to_count; ++i)
      func_type.to[i] = types[(uint64_t)(from_count) + i];


    return func_type;
}


static bool func_types_are_equal(FuncType* a, FuncType* b) {
  if (a->from_count != b->from_count || a->to_count != b->to_count)
    return 0;
  uint32_t i;
  for (i = 0; i < a->from_count; ++i)
    if (a->from[i] != b->from[i])
      return 0;
  for (i = 0; i < a->to_count; ++i)
    if (a->to[i] != b->to[i])
      return 0;
  return 1;
}

uint32_t mswasm_register_func_type(WasmModule* ctx, FuncType func_type) {

  // 1) scan func types table, if equal, and return index.
  for (int i = 0; i < ctx->func_type_count; ++i) {
    FuncType* func_types = ctx->func_type_table;
    if (func_types_are_equal(&func_types[i], &func_type)) {
      if (func_type.from) {
        free(func_type.from);
      }
      if (func_type.to) {
        free(func_type.to);
      }
      return i + 1;
    }
  }
  // 2) add to func type table if it is not already there
  uint32_t idx = ctx->func_type_count++;
  // realloc works fine even if *p_func_type_structs is null
  ctx->func_type_table = realloc(ctx->func_type_table,
                                 ctx->func_type_count * sizeof(FuncType));
  ctx->func_type_table[idx] = func_type;
  return idx + 1;
}


u32 lookup_func_index(WasmModule* ctx_ptr, u32 param_count, u32 result_count, Handle types) {
  //assert(false && "unimplemented");
  WasmModule* const ctx = (WasmModule* const) ctx_ptr;
  FuncType func_type = mswasm_create_func_type(param_count, result_count, types);
  return mswasm_register_func_type(ctx, func_type);
}
