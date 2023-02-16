

// definitions needed by rlbox 


typedef WasmModule* (*new_wasm_module_t)(i32 argc, Handle argv);
typedef void (*destroy_wasm_module_t)(WasmModule* ctx);
typedef void (*remove_callback_t)(WasmModule* ctx, u32 callback_idx);
typedef u32 (*add_callback_t)(WasmModule* ctx, u32 func_type_idx, Handle func_ptr);

WasmModule* new_wasm_module(i32 argc, Handle argv);
void destroy_wasm_module(WasmModule* ctx);
void remove_mswasm_callback(WasmModule* ctx, u32 callback_idx);
u32 add_mswasm_callback(WasmModule* ctx, u32 func_type_idx, Handle func_ptr);


typedef struct mswasm_sandbox_funcs_t {
  new_wasm_module_t create_mswasm_sandbox;
  destroy_wasm_module_t destroy_mswasm_sandbox;
  add_callback_t add_mswasm_callback;
  remove_callback_t remove_mswasm_callback;
} mswasm_sandbox_funcs_t;

mswasm_sandbox_funcs_t get_mswasm_sandbox_info() {
  mswasm_sandbox_funcs_t ret;
  //ret.wasm_rt_sys_init = &wasm_rt_sys_init;
  ret.create_mswasm_sandbox = &new_wasm_module; // rwasm naming convention
  ret.destroy_mswasm_sandbox = &destroy_wasm_module; // rwasm naming convention
  // ret.lookup_wasm2c_nonfunc_export = &lookup_wasm2c_nonfunc_export;
  // ret.lookup_wasm2c_func_index = &lookup_wasm2c_func_index;
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

