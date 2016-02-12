#pragma once

#include <string>
#include <sstream>
#include <unordered_set>
#include <Meta/TypeFactory.h>
#include "Meta/MetaEntities.h"
#include "DocSetManager.h"

namespace TypeScript {
class DefinitionWriter : Meta::MetaVisitor {
public:
    DefinitionWriter(std::pair<clang::Module*, std::vector<Meta::Meta*> >& module, Meta::TypeFactory& typeFactory, std::string docSetPath)
        : _module(module)
        , _typeFactory(typeFactory)
        , _docSet(docSetPath)
    {
    }

    std::string write();

    virtual void visit(Meta::InterfaceMeta* meta) override;

    virtual void visit(Meta::ProtocolMeta* meta) override;

    virtual void visit(Meta::CategoryMeta* meta) override;

    virtual void visit(Meta::FunctionMeta* meta) override;

    virtual void visit(Meta::StructMeta* meta) override;

    virtual void visit(Meta::UnionMeta* meta) override;

    virtual void visit(Meta::EnumMeta* meta) override;

    virtual void visit(Meta::VarMeta* meta) override;

    virtual void visit(Meta::MethodMeta* meta) override;

    virtual void visit(Meta::PropertyMeta* meta) override;

    virtual void visit(Meta::EnumConstantMeta* meta) override;

private:
    template <class Member>
    using CompoundMemberMap = std::map<std::string, std::pair<Meta::BaseClassMeta*, Member*> >;

    void writeMembers(const std::vector<Meta::RecordField>& fields, std::vector<TSComment> fieldsComments);
    void writeProperty(Meta::PropertyMeta* meta, Meta::BaseClassMeta* owner, Meta::InterfaceMeta* target, CompoundMemberMap<Meta::PropertyMeta> compoundProperties);

    static void getMembersRecursive(Meta::ProtocolMeta* protocol,
        CompoundMemberMap<Meta::MethodMeta>* staticMethods,
        CompoundMemberMap<Meta::PropertyMeta>* properties,
        CompoundMemberMap<Meta::MethodMeta>* instanceMethods,
        std::set<Meta::ProtocolMeta*>& visitedProtocols);

    static std::string writeConstructor(const CompoundMemberMap<Meta::MethodMeta>::value_type& initializer,
        const Meta::BaseClassMeta* owner);
    static std::string writeMethod(Meta::MethodMeta* meta, Meta::BaseClassMeta* owner);
    static std::string writeMethod(CompoundMemberMap<Meta::MethodMeta>::value_type& method, Meta::BaseClassMeta* owner,
        const std::set<Meta::ProtocolMeta*>& protocols);
    static std::string writeProperty(Meta::PropertyMeta* meta, Meta::BaseClassMeta* owner, bool optOutTypeChecking = false);
    static std::string writeFunctionProto(const std::vector<Meta::Type*>& signature);
    static std::string localizeReference(const std::string& jsName, std::string moduleName);
    static std::string localizeReference(const Meta::Meta& meta);
    static std::string tsifyType(const Meta::Type& type);
    static std::string computeMethodReturnType(const Meta::Type* retType, const Meta::BaseClassMeta* owner);
    std::string getTypeArgumentsStringOrEmpty(const clang::ObjCObjectType* objectType);

    static bool hasClosedGenerics(const Meta::Type& type);

    std::pair<clang::Module*, std::vector<Meta::Meta*> >& _module;
    Meta::TypeFactory& _typeFactory;
    DocSetManager _docSet;
    std::unordered_set<std::string> _importedModules;
    std::ostringstream _buffer;
};
}
