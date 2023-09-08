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
