using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalcHelper
{
    class Program
    {
        static UInt64 SMD(UInt64 SFS, UInt64 SB)
        {
            return SFS / SB - 2 * (UInt64) Math.Ceiling(SFS / (decimal) (2 * SB * SB));
        }

        static void Main(string[] args)
        {
            UInt64 sb = 128;
            for (UInt64 i = 1024; i < 1024*1024; i*=2)
            {
                Console.WriteLine($"SFS = {i} SB = {sb} SMD =  {SMD(i, sb)} CA = {i - SMD(i, sb) * sb}");
            }

            {
                UInt64 i = 2816;
                Console.WriteLine($"SFS = {i} SB = {sb} SMD =  {SMD(i, sb)} CA = {i - SMD(i, sb) * sb}");
            }
        }
    }
}
