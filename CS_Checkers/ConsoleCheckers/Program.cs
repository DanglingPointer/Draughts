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

            // Testing Piececontrollers
            IPiececontroller wpc = new WhitePiececontroller(bb);
            IPiececontroller bpc = new BlackPiececontroller(bb);
            if (wpc.CanMoveRight(new Pos(2, 3)))
                wpc.MoveRight(new Pos(2, 3));
            if (bpc.CanMoveRight(new Pos(5, 2)))
                bpc.MoveRight(new Pos(5, 2));
            bb.SetAt(7, 4, Piece.Empty);
            if (bpc.CanMoveLeft(new Pos(5, 6)))
                bpc.MoveLeft(new Pos(5, 6));

            Aux.Print(copy);

            if (wpc.CanJumpLeft(new Pos(3, 4)))
                wpc.JumpLeft(new Pos(3, 4));

            Aux.Print(copy);

            Direction d = wpc.CanJumpKing(new Pos(7,4));
            wpc.JumpKing(d, new Pos(7, 4));
            Aux.Print(copy);

            // 5, 4 right move
            if (bpc.CanMoveRight(new Pos(5, 4)))
                bpc.MoveRight(new Pos(5, 4));
            Aux.Print(copy);

            if (bpc.CanMoveRight(new Pos(6, 1)))
                bpc.MoveRight(new Pos(6, 1));
            if (bpc.CanMoveRight(new Pos(5, 2)))
                bpc.MoveRight(new Pos(5, 2));
            if (bpc.CanMoveRight(new Pos(7, 0)))
                bpc.MoveRight(new Pos(7, 0));
            Aux.Print(copy);

            bb.SetAt(3, 4, Piece.Empty);
            bb.SetAt(1, 2, Piece.White | Piece.King);
            bb.SetAt(2, 3, Piece.Black);
            d = wpc.CanJumpKing(new Pos(1, 2));
            wpc.JumpKing(d, new Pos(1, 2));
            Aux.Print(copy);

            ////3,4 left
            //d = wpc.CanMoveKing(new Pos(3, 4));
            //wpc.MoveKing(Direction.LeftUp, new Pos(3, 4));
            //Aux.Print(copy);

            //// 5, 4 jump left
            //if (bpc.CanJumpLeft(new Pos(5, 4)))
            //    bpc.JumpLeft(new Pos(5, 4));
            //Aux.Print(copy);

            //// 2, 5 move left
            //if (wpc.CanMoveLeft(new Pos(2, 5)))
            //    wpc.MoveLeft(new Pos(2, 5));
            //Aux.Print(copy);

            ////// 2, 1 jump right
            ////if (wpc.CanJumpRight(new Pos(2, 1)))
            ////    wpc.JumpRight(new Pos(2, 1));
            ////Aux.Print(copy);

            //// 1, 4, move left
            //if (wpc.CanMoveLeft(new Pos(1, 4)))
            //    wpc.MoveLeft(new Pos(1, 4));
            //Aux.Print(copy);
            //if (wpc.CanMoveLeft(new Pos(1, 4)))
            //    wpc.MoveLeft(new Pos(1, 4));
            //Aux.Print(copy);


            Console.ReadKey();
        }
    }
}
