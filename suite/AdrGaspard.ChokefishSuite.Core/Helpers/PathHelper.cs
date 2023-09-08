using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class PathHelper
    {
        public static string ToWslPath(this string source)
        {
            string[] pathParts = source.Replace('\\', '/').Split(':', 2);
            return $"/mnt/{pathParts[0].ToLower()}{pathParts[1]}";
        }
    }
}
