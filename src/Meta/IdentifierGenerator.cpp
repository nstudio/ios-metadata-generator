#include <clang/AST/Decl.h>
#include <sstream>
#include <clang/AST/DeclObjC.h>
#include <iostream>
#include "IdentifierGenerator.h"

using namespace std;

// TODO: this static maps contains hardcoded symbol names from the iOS SDK for which we know there is conflict
// in their JS name and must be renamed. Maybe there is better way to pass this map to the IdentifierGenerator. For
// third-party libraries the map may be parsed from file or something more flexible.
static map<clang::Decl::Kind, vector<string>> IosSdkNamesToRecalculate = {
        { clang::Decl::Kind::Record, { "kevent", "flock", "sigvec", "sigaction", "wait" } },
        { clang::Decl::Kind::Var, { "timezone" } },
        { clang::Decl::Kind::ObjCProtocol, { "NSObject", "AVVideoCompositionInstruction", "OS_dispatch_data" } }
};

map<clang::Decl::Kind, vector<string>>& Meta::IdentifierGenerator::getIosSdkNamesToRecalculate() {
    return IosSdkNamesToRecalculate;
}

void splitString(const std::string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::string Meta::IdentifierGenerator::getJsName(clang::Decl& decl) {
    std::string originalName = calculateOriginalName(decl);
    std::string jsName = calculateJsName(decl, originalName);
    std::vector<std::string> namesToCheck = _namesToRecalculate[decl.getKind()];
    if(std::find(namesToCheck.begin(), namesToCheck.end(), originalName) != namesToCheck.end()) {
        jsName = recalculateJsName(decl, jsName);
    }
    return jsName;
}

std::string Meta::IdentifierGenerator::getJsNameOrEmpty(clang::Decl& decl) {
    try {
        return getJsName(decl);
    }
    catch(IdentifierCreationException& e) {
        return "";
    }
}

std::string Meta::IdentifierGenerator::getModuleName(clang::Decl& decl) {
    return getModule(decl)->getFullModuleName();
}

std::string Meta::IdentifierGenerator::getModuleNameOrEmpty(clang::Decl& decl) {
    clang::Module *module = getModuleOrNull(decl);
    return module ? module->getFullModuleName() : "";
}

clang::Module *Meta::IdentifierGenerator::getModule(clang::Decl& decl) {
    clang::Module *owningModule = _astUnit->getPreprocessor().getHeaderSearchInfo().findModuleForHeader(getFileEntry(decl)).getModule();
    if(!owningModule)
        throw IdentifierCreationException(getJsNameOrEmpty(decl), getFileNameOrEmpty(decl), "Can't find module for this file name.");
    return owningModule;
}

clang::Module *Meta::IdentifierGenerator::getModuleOrNull(clang::Decl& decl) {
    const clang::FileEntry *entry = getFileEntryOrNull(decl);
    return entry ? _astUnit->getPreprocessor().getHeaderSearchInfo().findModuleForHeader(entry).getModule() : nullptr;
}

std::string Meta::IdentifierGenerator::getFileName(clang::Decl& decl) {
    return getFileEntry(decl)->getName();
}

std::string Meta::IdentifierGenerator::getFileNameOrEmpty(clang::Decl& decl) {
    if(const clang::FileEntry *entry = getFileEntryOrNull(decl))
        return entry->getName();
    return "";
}

const clang::FileEntry *Meta::IdentifierGenerator::getFileEntry(clang::Decl& decl) {
    if(const clang::FileEntry *entry = getFileEntryOrNull(decl))
        return entry;
    throw IdentifierCreationException(getJsNameOrEmpty(decl), "", "The containing file of declaration was not found.");
}

const clang::FileEntry *Meta::IdentifierGenerator::getFileEntryOrNull(clang::Decl& decl) {
    clang::FileID id = _astUnit->getSourceManager().getDecomposedLoc(decl.getLocation()).first;
    const clang::FileEntry *entry = _astUnit->getSourceManager().getFileEntryForID(id);
    return entry ? entry : nullptr;
}

Meta::FQName Meta::IdentifierGenerator::getFqName(clang::Decl& decl) {
    FQName fqName;
    fqName.jsName = getJsName(decl);
    fqName.module = getModuleName(decl);
    return fqName;
}

Meta::FQName Meta::IdentifierGenerator::getFqNameOrEmpty(clang::Decl& decl) {
    FQName fqName;
    fqName.jsName = getJsNameOrEmpty(decl);
    fqName.module = getModuleNameOrEmpty(decl);
    return fqName;
}

Meta::Identifier Meta::IdentifierGenerator::getIdentifier(clang::Decl& decl) {
    Identifier id;
    id.name = getJsName(decl);
    id.module = getModuleName(decl);
    id.fileName = getFileName(decl);
    return id;
}

Meta::Identifier Meta::IdentifierGenerator::getIdentifierOrEmpty(clang::Decl& decl) {
    Identifier id;
    id.name = getJsNameOrEmpty(decl);
    id.module = getModuleNameOrEmpty(decl);
    id.fileName = getFileNameOrEmpty(decl);
    return id;
}


string Meta::IdentifierGenerator::calculateOriginalName(clang::Decl& decl) {

    switch(decl.getKind()) {
        case clang::Decl::Kind::Function :
        case clang::Decl::Kind::ObjCInterface :
        case clang::Decl::Kind::ObjCProtocol :
        case clang::Decl::Kind::ObjCCategory :
        case clang::Decl::Kind::ObjCProperty :
        case clang::Decl::Kind::Field :
        case clang::Decl::Kind::EnumConstant :
        case clang::Decl::Kind::Var :
            return clang::dyn_cast<clang::NamedDecl>(&decl)->getNameAsString();
        case clang::Decl::Kind::ObjCMethod : {
            if(clang::ObjCMethodDecl *method = clang::dyn_cast<clang::ObjCMethodDecl>(&decl)) {
                return method->getSelector().getAsString();
            }
            throw logic_error("Invalid declaration.");
        }
        case clang::Decl::Kind::Record : {
            if(clang::RecordDecl *record = clang::dyn_cast<clang::RecordDecl>(&decl)) {
                if(!record->hasNameForLinkage()) {
                    // record->hasNameForLinkage() - http://clang.llvm.org/doxygen/classclang_1_1TagDecl.html#aa0c620992e6aca248368dc5c7c463687 (description what this method does)
                    throw IdentifierCreationException("[anonymous_record]", getFileNameOrEmpty(decl), "Anonymous record declared outside typedef. There is no suitable name for this declarations.");
                }
                if(clang::TypedefNameDecl *typedefDecl = record->getTypedefNameForAnonDecl()) {
                    return typedefDecl->getNameAsString();
                }
                return record->getNameAsString();
            }
            throw logic_error("Invalid declaration.");
        }
        case clang::Decl::Kind::Enum : {
            if(clang::EnumDecl *enumDecl = clang::dyn_cast<clang::EnumDecl>(&decl)) {
                if(!enumDecl->hasNameForLinkage()) {
                    // record->hasNameForLinkage() - http://clang.llvm.org/doxygen/classclang_1_1TagDecl.html#aa0c620992e6aca248368dc5c7c463687 (description what this method does)
                    throw IdentifierCreationException("[anonymous_enum]", getFileNameOrEmpty(decl) , "Anonymous enum declared outside typedef. There is no suitable name for this declarations.");
                }
                if(clang::TypedefNameDecl *typedefDecl = enumDecl->getTypedefNameForAnonDecl()) {
                    return typedefDecl->getNameAsString();
                }
                return enumDecl->getNameAsString();
            }
            throw logic_error("Invalid declaration.");
        }
        default:
            throw logic_error(string("Can't generate original name for ") + decl.getDeclKindName() + " type of declaration.");
    }
}

string Meta::IdentifierGenerator::calculateJsName(clang::Decl& decl, std::string originalName) {
    switch(decl.getKind()) {
        case clang::Decl::Kind::Record :
        case clang::Decl::Kind::Enum :
        case clang::Decl::Kind::Function :
        case clang::Decl::Kind::ObjCInterface :
        case clang::Decl::Kind::ObjCProtocol :
        case clang::Decl::Kind::ObjCCategory :
        case clang::Decl::Kind::ObjCProperty :
        case clang::Decl::Kind::Field :
        case clang::Decl::Kind::EnumConstant :
        case clang::Decl::Kind::Var :
            return originalName;
        case clang::Decl::Kind::ObjCMethod : {
            vector<string> tokens;
            splitString(originalName, ':', tokens);
            for (vector<string>::size_type i = 1; i < tokens.size(); ++i) {
                tokens[i][0] = toupper(tokens[i][0]);
                tokens[0] += tokens[i];
            }
            return tokens[0];
        }
        default:
            throw logic_error("Can't generate JS name for that type of declaration.");
    }
}

string Meta::IdentifierGenerator::recalculateJsName(clang::Decl& decl, std::string calculatedJsName) {
    switch(decl.getKind()) {
        case clang::Decl::Kind::Record : {
            clang::RecordDecl *record = llvm::dyn_cast<clang::RecordDecl>(&decl);
            return calculatedJsName + (record->isStruct() ? "Struct" : "Union");
        }
        case clang::Decl::Kind::Function :
            return calculatedJsName + "Function";
        case clang::Decl::Kind::Enum :
            return calculatedJsName + "Enum";
        case clang::Decl::Kind::ObjCInterface :
            return calculatedJsName + "Interface";
        case clang::Decl::Kind::ObjCProtocol :
            return calculatedJsName + "Protocol";
        case clang::Decl::Kind::ObjCCategory :
            return calculatedJsName + "Category";
        case clang::Decl::Kind::ObjCMethod :
            return calculatedJsName + "Method";
        case clang::Decl::Kind::ObjCProperty :
            return calculatedJsName + "Property";
        case clang::Decl::Kind::Var :
            return calculatedJsName + "Var";
        case clang::Decl::Kind::Field :
            return calculatedJsName + "Field";
        case clang::Decl::Kind::EnumConstant :
            return calculatedJsName + "Field";
        default:
            throw logic_error("Can't recalculate JS name for that type of declaration.");

    }
}
