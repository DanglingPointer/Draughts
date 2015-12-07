using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Checkers
{
    class ConsoleProgram
    {
        static void Main(string[] args)
        {
            Gameplay game = new Gameplay(C.Black, 10);
            bool cont;
            do
            {
                Aux.Print(game.Board);
                Console.WriteLine("Enter [row col direction]");
                Console.WriteLine("Direction: ru, rd, lu, ld");
                string[] inputs = Console.ReadLine().Split(' ');
                int row = Convert.ToInt32(inputs[0]);
                int col = Convert.ToInt32(inputs[1]);
                Direction dirn;
                switch (inputs[2])
                {
                    case "ru":
                        dirn = Direction.RightUp;
                        break;
                    case "rd":
                        dirn = Direction.RightDown;
                        break;
                    case "lu":
                        dirn = Direction.LeftUp;
                        break;
                    case "ld":
                        dirn = Direction.LeftDown;
                        break;
                    default:
                        dirn = Direction.None;
                        break;
                }
                game.PlayerTurn(new Pos(row, col), dirn);
                Aux.Print(game.Board);
                Console.WriteLine("\nAI thinking . . . . . . . .\n");
                cont = game.AITurn();
            } while (cont);


            Console.ReadKey();
        }
    }
}
