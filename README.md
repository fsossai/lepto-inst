# LeptoInst

_LLVM instructions have never been so concise_

__Before__: 
```c++
errs() << I1 << "\n";
errs() << I2 << "\n";
```

```llvm
%303 = bitcast %"class.std::vector.131"* %4 to %"struct.std::_Vector_base.132"*
%304 = getelementptr inbounds %"struct.std::_Vector_base.132", %"struct.std::_Vector_base.132"* %303, i32 0, i32 0
```
__After__:
```c++
LeptoInstVisitor LIV;
errs() << LIV.visit(I1) << "\n";
errs() << LIV.visit(I2) << "\n";
```

```llvm
%303 = bitcast %4
%304 = gep %303
```
