# LeptoInst

_LLVM instructions have never been so concise_

LeptoInst is an LLVM instruction visitor that renders IR in a shorter, more readable form.
The concise output emphasizes data flow while minimizing type information, attributes, and metadata.

**Without LeptoInst:**

```c++
errs() << I << "\n";
```

```llvm
%124 = call noalias noundef nonnull dereferenceable(40) ptr @_Znwm(i64 noundef 40) #15, !noalias !38
store ptr null, ptr %124, align 8, !tbaa !21, !noalias !38
%125 = getelementptr inbounds nuw i8, ptr %124, i64 28
store i32 %72, ptr %125, align 4, !tbaa !41, !noalias !38
%126 = getelementptr inbounds nuw i8, ptr %124, i64 32
store i32 0, ptr %126, align 8, !tbaa !43, !noalias !38
%127 = getelementptr inbounds nuw i8, ptr %124, i64 16
call void @_ZNSt3__127__tree_balance_after_insertB9nqn220108IPNS_16__tree_node_baseIPvEEEEvT_S5_(ptr noundef nonnull %122, ptr noundef nonnull %122) #16
store ptr %119, ptr %127, align 8, !tbaa !22
store ptr %124, ptr %120, align 8, !tbaa !25
%128 = load ptr, ptr %8, align 8, !tbaa !9
%129 = load ptr, ptr %128, align 8, !tbaa !21
%130 = icmp eq ptr %129, null
br i1 %130, label %132, label %131
```

**With LeptoInst:**

```c++
LeptoInstVisitor lepto;
errs() << lepto(I) << "\n";
```

```llvm
%124 = call operator new (40)
store ptr null to %124
%125 = gep %124, 28
store %72 to %125
%126 = gep %124, 32
store 0 to %126
%127 = gep %124, 16
call void std::__1::__tree_balance_after_insert (%122, %122)
store %119 to %127
store %124 to %120
%128 = load %8
%129 = load %128
%130 = icmp eq %129, ptr null
br %130, %132, %131
```

## Build and run

LeptoInst supports LLVM's new pass manager and retains its legacy pass
registration for older LLVM releases. Compatibility snapshots are available
through tags including `llvm14-compatible` and `llvm16-compatible`.

Build the plugin against the same LLVM installation that provides `opt`:

```sh
cmake -S . -B build -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm
cmake --build build
```

To try it immediately, compile the included route-finding example to LLVM
bitcode and run the pass over it:

```sh
make demo
```

Run it with the new pass manager:

```sh
opt -load-pass-plugin=build/src/LeptoInst.so \
  -passes=lepto-inst -disable-output input.ll
```
