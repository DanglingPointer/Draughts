#define CONSOLE
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
            Gameplay game = new Gameplay(C.Black);
            bool cont;
            Console.WriteLine("Direction:\nru = right up\nrd = right down\nlu = left up\nld = left down\n");
            do
            {
                Aux.Print(game.Board);
                Console.WriteLine("Enter [row col direction]");
                string[] inputs = Console.ReadLine().Split(' ');
                if (inputs.Length < 3)
                    return;
                uint row = Convert.ToUInt32(inputs[0]);
                uint col = Convert.ToUInt32(inputs[1]);
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
