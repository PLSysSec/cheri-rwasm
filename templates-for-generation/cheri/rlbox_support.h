

// definitions needed by rlbox 


typedef WasmModule* (*new_wasm_module_t)(i32 argc, Handle argv);
typedef void (*destroy_wasm_module_t)(WasmModule* ctx);


WasmModule* new_wasm_module(i32 argc, Handle argv);
void destroy_wasm_module(WasmModule* ctx);



typedef struct mswasm_sandbox_funcs_t {
  new_wasm_module_t create_mswasm_sandbox;
  destroy_wasm_module_t destroy_mswasm_sandbox;
  // add_callback_t add_mswasm_callback;
  // remove_callback_t remove_mswasm_callback;
} mswasm_sandbox_funcs_t;

mswasm_sandbox_funcs_t get_mswasm_sandbox_info() {
  mswasm_sandbox_funcs_t ret;
  //ret.wasm_rt_sys_init = &wasm_rt_sys_init;
  ret.create_mswasm_sandbox = &new_wasm_module; // rwasm naming convention
  ret.destroy_mswasm_sandbox = &destroy_wasm_module; // rwasm naming convention
  // ret.lookup_wasm2c_nonfunc_export = &lookup_wasm2c_nonfunc_export;
  // ret.lookup_wasm2c_func_index = &lookup_wasm2c_func_index;
  // ret.add_wasm2c_callback = &add_wasm2c_callback;
  // ret.remove_wasm2c_callback = &remove_wasm2c_callback;
  return ret;
}





// // typedef uint32_t (*lookup_wasm2c_func_index_t)(WasmModule* ctx,
// //                                                uint32_t param_count,
// //                                                uint32_t result_count,
// //                                                wasm_rt_type_t* types);
// // typedef uint32_t (*add_wasm2c_callback_t)(
// //     WasmModule* ctx,
// //     uint32_t func_type_idx,
// //     void* func_ptr,
// //     wasm_rt_elem_target_class_t func_class);
// // typedef void (*remove_wasm2c_callback_t)(WasmModule* ctx, uint32_t callback_idx);


// typedef struct mswasm_sandbox_funcs_t {
//   new_wasm_module_t new_wasm_module;
//   // destroy_wasm_module_t destroy_wasm_module;
//   // add_callback_t add_mswasm_callback;
//   // remove_callback_t remove_mswasm_callback;
// } mswasm_sandbox_funcs_t;

