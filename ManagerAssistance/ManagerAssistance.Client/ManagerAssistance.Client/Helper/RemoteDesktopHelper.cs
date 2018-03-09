using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using ManagerAssistance.Common.Helpers;
using ManagerAssistance.Common.Network.Protocol;
using ManagerAssistance.Client.Helper.Native.Impl;

namespace ManagerAssistance.Client.Helper
{
    public static class RemoteDesktopHelper
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="screenNb"></param>
        /// <param name="quality"></param>
        /// <returns></returns>
        public static ScreenCaptureMessage CaptureScreen(int screenNb, int quality)
        {
            var bounds = Screen.AllScreens[screenNb].Bounds;
            var screen = new Bitmap(
                bounds.Width,
                bounds.Height,
                PixelFormat.Format32bppPArgb);

            using (var dest = Graphics.FromImage(screen))
            {
                var destPtr = dest.GetHdc();
                var srcPtr = Gdi32.CreateDC("DISPLAY",
                    null,
                    null,
                    IntPtr.Zero);

                Gdi32.BitBlt(destPtr,
                    0,
                    0,
                    bounds.Width,
                    bounds.Height,
                    srcPtr,
                    bounds.X,
                    bounds.Y,
                    Gdi32.ROP_COPY);

                Gdi32.DeleteDC(srcPtr);
                dest.ReleaseHdc(destPtr);
            }

            var compressed = ImageHelpers.CompressImage(screen, quality);
            screen.Dispose();

            return new ScreenCaptureMessage
            {
                ScreenNumber = screenNb,
                Quality = quality,
                Width = bounds.Width,
                Height = bounds.Height,
                RawImage = compressed
            };
        }
    }
}
