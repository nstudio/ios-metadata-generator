﻿using System;
using MetadataGenerator.Core.Ast;
using System.Collections.ObjectModel;
using MetadataGenerator.Core.Meta.Filters;
using System.Diagnostics;

namespace MetadataGenerator.Core.Meta.Utils
{
    [DebuggerDisplay("{ModuleName} Count: {Count}")]
    public class ModuleDeclarationsContainer : Collection<IDeclaration>
    {
        public ModuleDeclarationsContainer(string moduleName)
        {
            this.ModuleName = moduleName;
        }

        public string ModuleName { get; private set; }

        public void Apply(params IMetaFilter[] filters)
        {
            foreach (IMetaFilter filter in filters)
            {
                filter.Filter(this);
            }
        }
    }
}
