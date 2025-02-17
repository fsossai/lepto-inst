# LeptoInst

_LLVM instructions have never been so concise_

__Before__: 
```c++
errs() << I << "\n";
```

```llvm
%.0430 = phi i32 [ 0, %_ZN11BuilderBaseIiiiLb1EEC2ERK6CLBase.exit ], [ %253, %_ZN7pvectorIiED2Ev.exit.i ]
%303 = bitcast %"class.std::vector.131"* %4 to %"struct.std::_Vector_base.132"*
%304 = getelementptr inbounds %"struct.std::_Vector_base.132", %"struct.std::_Vector_base.132"* %303, i32 0, i32 1
call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_(%"class.std::__cxx11::basic_string"* noundef nonnull align 8 dereferenceable(32) %20, i8* noundef %28, %"class.std::allocator.16"* noundef nonnull align 1 dereferenceable(1) %21)
```
__After__:
```c++
LeptoInstVisitor lepto;
errs() << lepto(I) << "\n";
```

```llvm
%.0430 = phi 0, %253
%303 = bitcast %4
%304 = gep %303, 0, 1
call std::__cxx11::basic_string (%20, %28, %21)
```
