using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Checkers
{
    interface IBase
    {
        void foo();
    }
    class A : IBase
    {
        public void foo()
        {
            Console.WriteLine("A");
        }
    }
    class B : IBase
    {
        public void foo()
        {
            Console.WriteLine("B");
        }
    }

    class ConsoleProgram
    {
        static void Main(string[] args)
        {
            Piece[] ps = new Piece[10];
            ps[0] = Piece.Black | Piece.King;
            foreach (Piece p in ps)
                Console.WriteLine(p);

            IBase obj = new B();
            if (obj is A)
                Console.WriteLine("object is A");
            if (obj is B)
                Console.WriteLine("Object is B");

            Console.ReadKey();
        }
    }
}
