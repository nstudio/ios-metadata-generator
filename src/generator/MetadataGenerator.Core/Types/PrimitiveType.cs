using MetadataGenerator.Core.Ast;
using System;
using System.Linq;

namespace MetadataGenerator.Core.Types
{
    public class PrimitiveType : TypeDefinition
    {
        public PrimitiveTypeType Type { get; set; }

        public PrimitiveType(PrimitiveTypeType type)
        {
            this.Type = type;
        }

        protected override bool? IsSupportedInternal(System.Collections.Generic.Dictionary<TypeDefinition, bool> typesCache, System.Collections.Generic.Dictionary<BaseDeclaration, bool> declarationsCache)
        {
            if (this.Type == PrimitiveTypeType.Int128 || this.Type == PrimitiveTypeType.UInt128)
            {
                return false;
            }

            return base.IsSupportedInternal(typesCache, declarationsCache);
        }

        internal override string ToStringInternal(string identifier, bool isOuter = false)
        {
            if (identifier.Length > 0)
            {
                identifier = " " + identifier;
            }
            return ToStringHelper() + this.Type.ToCTypeString() + identifier;
        }

        public override TypeEncoding ToTypeEncoding()
        {
            switch (this.Type)
            {
                case PrimitiveTypeType.Void:
                    return TypeEncoding.Void;
                case PrimitiveTypeType.Bool:
                    return TypeEncoding.Bool;
                case PrimitiveTypeType.Short:
                    return TypeEncoding.Short;
                case PrimitiveTypeType.UShort:
                    return TypeEncoding.UShort;
                case PrimitiveTypeType.Int:
                    return TypeEncoding.Int;
                case PrimitiveTypeType.UInt:
                    return TypeEncoding.UInt;
                    //case PrimitiveTypeType.Int128:
                    //    break;
                    //case PrimitiveTypeType.UInt128:
                    //    break;
                case PrimitiveTypeType.Long:
                    return TypeEncoding.Long;
                case PrimitiveTypeType.ULong:
                    return TypeEncoding.ULong;
                case PrimitiveTypeType.LongLong:
                    return TypeEncoding.LongLong;
                case PrimitiveTypeType.ULongLong:
                    return TypeEncoding.ULongLong;
                case PrimitiveTypeType.SChar:
                case PrimitiveTypeType.CharU:
                case PrimitiveTypeType.CharS:
                    return TypeEncoding.SignedChar;
                case PrimitiveTypeType.UChar:
                    return TypeEncoding.UnsignedChar;
                    //case PrimitiveTypeType.Char16:
                    //    break;
                    //case PrimitiveTypeType.Char32:
                    //    break;
                    //case PrimitiveTypeType.WChar:
                    //    break;
                case PrimitiveTypeType.Float:
                    return TypeEncoding.Float;
                case PrimitiveTypeType.Double:
                case PrimitiveTypeType.LongDouble:
                    // Objective-C does not support the long double type. @encode(long double) returns d, which is the same encoding as for double.
                    return TypeEncoding.Double;
                default :
                    // return TypeEncoding.Unknown;
                    throw new Exception("Unknown primitive type.");
            }
        }
    }
}
