#include "binaryTypeEncodingSerializer.h"

binary::MetaFileOffset binary::BinaryTypeEncodingSerializer::visit(std::vector<::Meta::Type>& types) {
    binary::MetaFileOffset offset = 0;
    if (types.size() > 0) {
        vector<unique_ptr<binary::TypeEncoding>> binaryEncodings;
        for (::Meta::Type &type : types) {
            unique_ptr<binary::TypeEncoding> binaryEncoding = type.visit(*this);
            binaryEncodings.push_back(std::move(binaryEncoding));
        }

        offset = this->_heapWriter.push_arrayCount(types.size());
        for (unique_ptr<binary::TypeEncoding>& binaryEncoding : binaryEncodings) {
            binaryEncoding->save(this->_heapWriter);
        }
    }
    return offset;
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUnknown() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Unknown));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitVoid() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Void));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitBool() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Bool));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitShort() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Short));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUShort(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::UShort));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitInt() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Int));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUInt() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::UInt));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitLong() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Long));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUlong(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::ULong));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitLongLong(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::LongLong));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitULongLong(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::ULongLong));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitSignedChar() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Char));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUnsignedChar() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::UChar));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUnichar() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Unichar));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitCString() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::CString));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitFloat() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Float));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitDouble() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Double));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitVaList(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::VaList));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitSelector() {
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Selector));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitInstancetype(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::InstanceType));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitClass(::Meta::ClassTypeDetails& typeDetails){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Class)); // TODO: Add protocols
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitProtocol(){
    return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Protocol));
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitId(::Meta::IdTypeDetails& typeDetails){
        return unique_ptr<binary::TypeEncoding>(new binary::TypeEncoding(binary::BinaryTypeEncodingType::Id)); // TODO: Add protocols
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitConstantArray(::Meta::ConstantArrayTypeDetails& typeDetails){
    binary::ConstantArrayEncoding* s = new binary::ConstantArrayEncoding();
    s->_size = typeDetails.size;
    s->_elementType = typeDetails.innerType.visit(*this);
    return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitIncompleteArray(::Meta::IncompleteArrayTypeDetails& typeDetails){
    binary::IncompleteArrayEncoding* s = new binary::IncompleteArrayEncoding();
    s->_elementType = typeDetails.innerType.visit(*this);
    return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitInterface(::Meta::InterfaceTypeDetails& typeDetails){
    binary::DeclarationReferenceEncoding* s = new binary::DeclarationReferenceEncoding(BinaryTypeEncodingType::InterfaceDeclarationReference);
    s->_name = this->_heapWriter.push_string(typeDetails.name.jsName);
    return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitPointer(::Meta::PointerTypeDetails& typeDetails){
        binary::PointerEncoding* s = new binary::PointerEncoding();
        s->_target = typeDetails.innerType.visit(*this);
        return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitBlock(::Meta::BlockTypeDetails& typeDetails){
        binary::BlockEncoding* s = new binary::BlockEncoding();
        s->_encodingsCount = (uint8_t)typeDetails.signature.size();
        for (::Meta::Type& type : typeDetails.signature) {
            s->_encodings.push_back(type.visit(*this));
        }
        return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitFunctionPointer(::Meta::FunctionPointerTypeDetails& typeDetails){
    binary::FunctionEncoding* s = new binary::FunctionEncoding();
    s->_encodingsCount = (uint8_t)typeDetails.signature.size();
    for (::Meta::Type& type : typeDetails.signature) {
        s->_encodings.push_back(type.visit(*this));
    }
    return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitStruct(::Meta::StructTypeDetails& typeDetails){
    binary::DeclarationReferenceEncoding* s = new binary::DeclarationReferenceEncoding(BinaryTypeEncodingType::StructDeclarationReference);
    s->_name = this->_heapWriter.push_string(typeDetails.name.jsName);
    return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitUnion(::Meta::UnionTypeDetails& typeDetails){
        binary::DeclarationReferenceEncoding* s = new binary::DeclarationReferenceEncoding(BinaryTypeEncodingType::UnionDeclarationReference);
        s->_name = this->_heapWriter.push_string(typeDetails.name.jsName);
        return unique_ptr<binary::TypeEncoding>(s);
}

// TODO: Remove it
unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitPureInterface(::Meta::PureInterfaceTypeDetails& typeDetails){
        binary::InterfaceDeclarationEncoding* s = new binary::InterfaceDeclarationEncoding();
        s->_name = this->_heapWriter.push_string(typeDetails.name.jsName);
        return unique_ptr<binary::TypeEncoding>(s);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitAnonymousStruct(::Meta::AnonymousStructTypeDetails& typeDetails){
    return this->serializeRecordEncoding(binary::BinaryTypeEncodingType::AnonymousStruct, typeDetails.fields);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::visitAnonymousUnion(::Meta::AnonymousUnionTypeDetails& typeDetails){
    return this->serializeRecordEncoding(binary::BinaryTypeEncodingType::AnonymousUnion, typeDetails.fields);
}

unique_ptr<binary::TypeEncoding> binary::BinaryTypeEncodingSerializer::serializeRecordEncoding(binary::BinaryTypeEncodingType encodingType, std::vector<::Meta::RecordField>& fields) {
    binary::AnonymousRecordEncoding* s = new binary::AnonymousRecordEncoding(encodingType);
    s->_fieldsCount = (uint8_t)fields.size();

    for (::Meta::RecordField& field : fields) {
        s->_fieldNames.push_back(this->_heapWriter.push_string(field.name));
    }

    for (::Meta::RecordField& field : fields) {
        s->_fieldEncodings.push_back(field.encoding.visit(*this));
    }
    return unique_ptr<binary::TypeEncoding>(s);
}