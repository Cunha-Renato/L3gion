
namespace L3gion
{
    public class Input
    {
        public static bool IsKeyDown(LgKey keycode)
        {
            return InternalCalls.Input_IskeyDown(keycode);
        }
    }
}
