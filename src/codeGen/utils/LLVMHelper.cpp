#include "LLVMHelper.hpp"
#include <utils/Out.hpp>
#include <ast/node/statement/expression/literal/IntLiteralNode.hpp>
#include <ast/node/statement/expression/literal/BoolLiteralNode.hpp>
#include <ast/node/statement/expression/literal/CharLiteralNode.hpp>
#include <ast/node/statement/expression/literal/NullNode.hpp>
#include <ast/node/statement/expression/literal/FloatLiteralNode.hpp>
#include <main.hpp>

#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Passes/PassBuilder.h>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

LLVMHelper::LLVMHelper(string moduleName) {
    TheContext = make_shared<LLVMContext>();
    TheModule = make_shared<Module>(moduleName, *TheContext);
    Builder = make_shared<IRBuilder<>>(*TheContext);
}

Type* LLVMHelper::getVoidType() {
    return Type::getVoidTy(*TheContext);
}

void LLVMHelper::prepareBuild() {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);

    string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target) {
        errs() << Error;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    TheTargetMachine =
            Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());
}

void LLVMHelper::printModule() {
    TheModule->print(errs(), nullptr);
}

void LLVMHelper::runPasses() {
    // Create the analysis managers.
    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;
    PassBuilder PB{TheTargetMachine};
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
    ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(OptimizationLevel::O2);
    MPM.run(*TheModule, MAM);
}

void LLVMHelper::build(raw_pwrite_stream &dest) {
    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
    }

    pass.run(*TheModule);
    dest.flush();
}

StructType *LLVMHelper::createStructType(string name) {
    return StructType::create(*TheContext, name);
}

GlobalVariable *LLVMHelper::createGlobalVar(Type *type, string name) {
    return new GlobalVariable(*TheModule, type, false, GlobalValue::LinkageTypes::ExternalLinkage, 0, name);
}

ConstantPointerNull *LLVMHelper::getNullptr(PointerType* type) {
    return ConstantPointerNull::get(type);
}

Function *LLVMHelper::getFunction(string name) {
    return TheModule->getFunction(name);
}

Function *LLVMHelper::createFunctionPrototype(string name, Type *ret, vector<Type *> args) {
    Function* f = getFunction(name);
    if (!f) {
        FunctionType* ft = FunctionType::get(ret, args, false);
        return Function::Create(ft, Function::ExternalLinkage, name, *TheModule);
    }
    return f;
}

BasicBlock *LLVMHelper::createBBinFunc(string name, Function *func) {
    BasicBlock *BB = BasicBlock::Create(*TheContext, name, func);
    return BB;
}

void LLVMHelper::activateBB(BasicBlock *BB) {
    Builder->SetInsertPoint(BB);
}

Value *LLVMHelper::createAlloca(Type *type, Value *arrSize, string name) {
    return Builder->CreateAlloca(type, arrSize, name);
}

void LLVMHelper::createStore(Value *val, Value *ptr) {
    Builder->CreateStore(val, ptr);
}

void LLVMHelper::createBr(BasicBlock *BB) {
    Builder->CreateBr(BB);
}

void LLVMHelper::createRet(Value *val) {
    Builder->CreateRet(val);
}

Value *LLVMHelper::createCall(Function *func, vector<Value *> args, string name) {
    return Builder->CreateCall(func, args, name);
}

Value *LLVMHelper::createCall(string func, vector<Value *> args, string name) {
    Function* f = getFunction(func);
    if (!f)
        Out::errorMessage("Can not create call instruction for " + func);
    return Builder->CreateCall(f, args, name);
}

Value *LLVMHelper::createLoad(Type *type, Value *ptr, string name) {
    return Builder->CreateLoad(type, ptr, name);
}

void LLVMHelper::createIfElse(Value *cond, BasicBlock *thenBB, BasicBlock *elseBB) {
    Builder->CreateCondBr(cond, thenBB, elseBB);
}

BasicBlock *LLVMHelper::getActiveBB() {
    return Builder->GetInsertBlock();
}

Function *LLVMHelper::getCurrFunction() {
    return getActiveBB()->getParent();
}

IntegerType *LLVMHelper::getIntType(int size) {
    return IntegerType::get(*TheContext, size);
}

ConstantInt *LLVMHelper::getConstInt(int size, uint64_t val) {
    return ConstantInt::get(getIntType(size), val, true);
}

Type *LLVMHelper::getFloatType() {
    return Type::getFloatTy(*TheContext);
}

Type *LLVMHelper::getDoubleType() {
    return Type::getDoubleTy(*TheContext);
}

Constant *LLVMHelper::getConstFloat(double val) {
    return ConstantFP::get(getFloatType(), val);
}

Constant *LLVMHelper::getConstDouble(double val) {
    return ConstantFP::get(getDoubleType(), val);;
}

PointerType *LLVMHelper::getPointerType(Type *type, int addressSpace) {
    return PointerType::get(type, addressSpace);
}

Value *LLVMHelper::createPtrToInt(Value *ptr, Type *intType, string name) {
    return Builder->CreatePtrToInt(ptr, intType, name);
}

Value *LLVMHelper::createGetElementPtr(Type *type, Value *ptr, vector<Value *> indexes, string name) {
    return GetElementPtrInst::Create(type, ptr, indexes, name, getActiveBB());
}

Value *LLVMHelper::createSizeof(Type *type) {
    return ConstantExpr::getSizeOf(type);
}

Value *LLVMHelper::createEQ(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_EQ, lhs, rhs, name);
}

Value *LLVMHelper::createFPEQ(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_OEQ, lhs, rhs, name);
}

Value *LLVMHelper::createNE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_NE, lhs, rhs, name);
}

Value *LLVMHelper::createFPNE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_ONE, lhs, rhs, name);
}

Value *LLVMHelper::createLT(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_SLT, lhs, rhs, name);
}

Value *LLVMHelper::createFPLT(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_OLT, lhs, rhs, name);
}

Value *LLVMHelper::createGT(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_SGT, lhs, rhs, name);
}

Value *LLVMHelper::createFPGT(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_OGT, lhs, rhs, name);
}

Value *LLVMHelper::createLE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_SLE, lhs, rhs, name);
}

Value *LLVMHelper::createFPLE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_OLE, lhs, rhs, name);
}

Value *LLVMHelper::createGE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateICmp(CmpInst::Predicate::ICMP_SGE, lhs, rhs, name);
}

Value *LLVMHelper::createFPGE(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFCmp(CmpInst::Predicate::FCMP_OGE, lhs, rhs, name);
}

Value *LLVMHelper::createAdd(Value *lhs, Value *rhs, string name) {
    return Builder->CreateAdd(lhs, rhs, name);
}

Value *LLVMHelper::createFPAdd(Value *lhs, Value *rhs, string name) {
    return Builder->CreateFAdd(lhs, rhs, name);
}

Value *LLVMHelper::createSub(Value *lhs, Value *rhs, string name) {
    Builder->CreateSub(lhs, rhs, name);
}

Value *LLVMHelper::createFPSub(Value *lhs, Value *rhs, string name) {
    Builder->CreateFSub(lhs, rhs, name);
}

Value *LLVMHelper::createMul(Value *lhs, Value *rhs, string name) {
    Builder->CreateMul(lhs, rhs, name);
}

Value *LLVMHelper::createFPMul(Value *lhs, Value *rhs, string name) {
    Builder->CreateFMul(lhs, rhs, name);
}

Value *LLVMHelper::createDiv(Value *lhs, Value *rhs, string name) {
    Builder->CreateSDiv(lhs, rhs, name);
}

Value *LLVMHelper::createFPDiv(Value *lhs, Value *rhs, string name) {
    Builder->CreateFDiv(lhs, rhs, name);
}

bool LLVMHelper::isFloatValue(Value *val) {
    return val->getType()->isFloatTy();
}

bool LLVMHelper::isDoubleValue(Value *val) {
    return val->getType()->isDoubleTy();
}

bool LLVMHelper::isFloatingPointValue(Value *val) {
    return isFloatValue(val) || isDoubleValue(val);
}

Value *LLVMHelper::castToDouble(Value *val, string name) {
    return Builder->CreateFPCast(val, getDoubleType(), name);
}

Value *LLVMHelper::castToFloat(Value *val, string name) {
    return Builder->CreateFPCast(val, getFloatType(), name);
}

string LLVMHelper::getModuleName() {
    return string(TheModule->getName());
}

ArrayType *LLVMHelper::getArrayType(Type *type, uint64_t n) {
    return ArrayType::get(type, n);
}