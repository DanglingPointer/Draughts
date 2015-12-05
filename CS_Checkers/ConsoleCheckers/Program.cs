using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Checkers
{
    class ConsoleProgram
    {
        static void Main(string[] args)
        {
            // Testing BoardBuilder
            Piece[] ps;
            BoardBuilder.Initialize(out ps);
            Aux.Print(ps);

            BoardBuilder bb = new BoardBuilder();
            bb.Field = ps;

            Console.WriteLine("position 2,3 is {0}", bb.GetAt(2, 3));
            Console.WriteLine("position 6,3 is {0}", bb.GetAt(6, 3));

            Console.WriteLine("setting black king at pos 2,3 and removing at 1,2");
            bb.SetAt(2, 3, Piece.Black | Piece.King);
            bb.SetAt(1, 2, Piece.Empty);
            Aux.Print(ps);
            Pos[] blacks = bb.WhitePositions;
            foreach(Pos p in blacks)
            {
                Console.WriteLine("white piece at pos {0}, {1}", p.Row, p.Col);
            }
            Piece[] copy = bb.Copy;
            Console.WriteLine("Counter =  {0}", bb.Counter);
            bb.ResetState();
            Console.WriteLine("Initial board after resetting:");
            Aux.Print(ps);
            Console.WriteLine("Copy:");
            Aux.Print(copy);
            Console.WriteLine("Counter =  {0}", bb.Counter);

            bb.Field = copy;
            Console.WriteLine("position 2,3 is {0}", bb.GetAt(2, 3));
            bb.Initialize();
            Console.WriteLine("Initialized copy:");
            Aux.Print(copy);
            Console.WriteLine("Counter =  {0}", bb.Counter);

            Pos[] cblacks = bb.BlackPositions;
            foreach (Pos p in cblacks)
            {
                Console.WriteLine("black piece at pos {0}, {1}", p.Row, p.Col);
            }


            Console.ReadKey();
        }
    }
}
