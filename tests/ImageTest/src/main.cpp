#include "structures/Image.hpp"
#include "utils/PVLog.hpp"

using PV::Image;

// Image::Image(filename.png24)
void testPng24Load() {

   // Expecting a 2x2x3 image with
   // the following layout:
   //
   // Red  Green
   // Blue White
   Image png24("input/rgb.png");

   bool tl = png24.getPixelR(0, 0) == 1.0f && png24.getPixelG(0, 0) == 0.0f
             && png24.getPixelB(0, 0) == 0.0f;

   bool tr = png24.getPixelR(1, 0) == 0.0f && png24.getPixelG(1, 0) == 1.0f
             && png24.getPixelB(1, 0) == 0.0f;

   bool bl = png24.getPixelR(0, 1) == 0.0f && png24.getPixelG(0, 1) == 0.0f
             && png24.getPixelB(0, 1) == 1.0f;

   bool br = png24.getPixelR(1, 1) == 1.0f && png24.getPixelG(1, 1) == 1.0f
             && png24.getPixelB(1, 1) == 1.0f;

   FatalIf(
         !tl,
         "Failed (Top Left). Expected (1.0, 0.0, 0.0), found (%f, %f, %f) instead.\n",
         (double)png24.getPixelR(0, 0),
         (double)png24.getPixelG(0, 0),
         (double)png24.getPixelB(0, 0));
   FatalIf(
         !tr,
         "Failed (Top Right). Expected (0.0, 1.0, 0.0), found (%f, %f, %f) instead.\n",
         (double)png24.getPixelR(1, 0),
         (double)png24.getPixelG(1, 0),
         (double)png24.getPixelB(1, 0));
   FatalIf(
         !bl,
         "Failed (Bottom Left). Expected (0.0, 0.0, 1.0), found (%f, %f, %f) instead.\n",
         (double)png24.getPixelR(0, 1),
         (double)png24.getPixelG(0, 1),
         (double)png24.getPixelB(0, 1));
   FatalIf(
         !br,
         "Failed (Bottom Right). Expected (1.0, 1.0, 1.0), found (%f, %f, %f) instead.\n",
         (double)png24.getPixelR(1, 1),
         (double)png24.getPixelG(1, 1),
         (double)png24.getPixelB(1, 1));
}

// Image;:Image(filename.png32)
void testPng32Load() {

   // Expecting a 2x2x4 image with
   // the following layout:
   //
   // Red  Green
   // Blue Transparent
   Image png32("input/rgba.png");

   bool tl = png32.getPixelR(0, 0) == 1.0f && png32.getPixelG(0, 0) == 0.0f
             && png32.getPixelB(0, 0) == 0.0f && png32.getPixelA(0, 0) == 1.0f;

   bool tr = png32.getPixelR(1, 0) == 0.0f && png32.getPixelG(1, 0) == 1.0f
             && png32.getPixelB(1, 0) == 0.0f && png32.getPixelA(1, 0) == 1.0f;

   bool bl = png32.getPixelR(0, 1) == 0.0f && png32.getPixelG(0, 1) == 0.0f
             && png32.getPixelB(0, 1) == 1.0f && png32.getPixelA(0, 1) == 1.0f;

   // Not all pngs store the color value of transparent pixels,
   // so we're only going to test that the alpha is set correctly.
   bool br = png32.getPixelA(1, 1) == 0.0f;

   FatalIf(
         !tl,
         "Failed (Top Left). Expected (1.0, 0.0, 0.0, 1.0), found (%f, %f, %f, %f) instead.\n",
         (double)png32.getPixelR(0, 0),
         (double)png32.getPixelG(0, 0),
         (double)png32.getPixelB(0, 0),
         (double)png32.getPixelA(0, 0));
   FatalIf(
         !tr,
         "Failed (Top Right). Expected (0.0, 1.0, 0.0, 1.0), found (%f, %f, %f, %f) instead.\n",
         (double)png32.getPixelR(1, 0),
         (double)png32.getPixelG(1, 0),
         (double)png32.getPixelB(1, 0),
         (double)png32.getPixelA(1, 0));
   FatalIf(
         !bl,
         "Failed (Bottom Left). Expected (0.0, 0.0, 1.0, 1.0), found (%f, %f, %f, %f) instead.\n",
         (double)png32.getPixelR(0, 1),
         (double)png32.getPixelG(0, 1),
         (double)png32.getPixelB(0, 1),
         (double)png32.getPixelA(0, 1));
   FatalIf(
         !br,
         "Failed (Bottom Right). Expected A = 0.0, found %f instead.\n",
         (double)png32.getPixelA(1, 1));
}

// Image::Image(filename.jpg)
void testJpgLoad() {

   // Expecting a 2x2x3 image with
   // the following layout:
   //
   // Red  Green
   // Blue White
   //
   // Because jpg is a lossy format, we're going to
   // check ranges instead of values.

   Image jpg("input/rgb.jpg");

   bool tl =
         jpg.getPixelR(0, 0) >= 0.9f && jpg.getPixelG(0, 0) <= 0.1f && jpg.getPixelB(0, 0) <= 0.1f;

   bool tr =
         jpg.getPixelR(1, 0) <= 0.1f && jpg.getPixelG(1, 0) >= 0.9f && jpg.getPixelB(1, 0) <= 0.1f;

   bool bl =
         jpg.getPixelR(0, 1) <= 0.1f && jpg.getPixelG(0, 1) <= 0.1f && jpg.getPixelB(0, 1) >= 0.9f;

   bool br =
         jpg.getPixelR(1, 1) >= 0.9f && jpg.getPixelG(1, 1) >= 0.9f && jpg.getPixelB(1, 1) >= 0.9f;

   FatalIf(
         !tl,
         "Failed (Top Left). Expected values within 0.1 of (1.0, 0.0, 0.0), found (%f, %f, %f) "
         "instead.\n",
         (double)jpg.getPixelR(0, 0),
         (double)jpg.getPixelG(0, 0),
         (double)jpg.getPixelB(0, 0));
   FatalIf(
         !tr,
         "Failed (Top Right). Expected values within 0.1 of (0.0, 1.0, 0.0), found (%f, %f, %f) "
         "instead.\n",
         (double)jpg.getPixelR(1, 0),
         (double)jpg.getPixelG(1, 0),
         (double)jpg.getPixelB(1, 0));
   FatalIf(
         !bl,
         "Failed (Bottom Left). Expected values within 0.1 of (0.0, 0.0, 1.0), found (%f, %f, %f) "
         "instead.\n",
         (double)jpg.getPixelR(0, 1),
         (double)jpg.getPixelG(0, 1),
         (double)jpg.getPixelB(0, 1));
   FatalIf(
         !br,
         "Failed (Bottom Right). Expected values within 0.1 of (1.0, 1.0, 1.0), found (%f, %f, %f) "
         "instead.\n",
         (double)jpg.getPixelR(1, 1),
         (double)jpg.getPixelG(1, 1),
         (double)jpg.getPixelB(1, 1));
}

// Image::Image(filename.bmp)
void testBmpLoad() {
   // Expecting a 2x2x3 image with
   // the following layout:
   //
   // Red  Green
   // Blue White
   Image bmp("input/rgb.bmp");

   bool tl =
         bmp.getPixelR(0, 0) == 1.0f && bmp.getPixelG(0, 0) == 0.0f && bmp.getPixelB(0, 0) == 0.0f;

   bool tr =
         bmp.getPixelR(1, 0) == 0.0f && bmp.getPixelG(1, 0) == 1.0f && bmp.getPixelB(1, 0) == 0.0f;

   bool bl =
         bmp.getPixelR(0, 1) == 0.0f && bmp.getPixelG(0, 1) == 0.0f && bmp.getPixelB(0, 1) == 1.0f;

   bool br =
         bmp.getPixelR(1, 1) == 1.0f && bmp.getPixelG(1, 1) == 1.0f && bmp.getPixelB(1, 1) == 1.0f;

   FatalIf(
         !tl,
         "Failed (Top Left). Expected (1.0, 0.0, 0.0), found (%f, %f, %f) instead.\n",
         (double)bmp.getPixelR(0, 0),
         (double)bmp.getPixelG(0, 0),
         (double)bmp.getPixelB(0, 0));
   FatalIf(
         !tr,
         "Failed (Top Right). Expected (0.0, 1.0, 0.0), found (%f, %f, %f) instead.\n",
         (double)bmp.getPixelR(1, 0),
         (double)bmp.getPixelG(1, 0),
         (double)bmp.getPixelB(1, 0));
   FatalIf(
         !bl,
         "Failed (Bottom Left). Expected (0.0, 0.0, 1.0), found (%f, %f, %f) instead.\n",
         (double)bmp.getPixelR(0, 1),
         (double)bmp.getPixelG(0, 1),
         (double)bmp.getPixelB(0, 1));
   FatalIf(
         !br,
         "Failed (Bottom Right). Expected (1.0, 1.0, 1.0), found (%f, %f, %f) instead.\n",
         (double)bmp.getPixelR(1, 1),
         (double)bmp.getPixelG(1, 1),
         (double)bmp.getPixelB(1, 1));
}

// Image::convertToColor(bool alphaChannel)
void testConvertToGray() {

   // Expects the same images from png24 and png32 tests. Uses
   // the constants from Image to check if the RGB values were
   // converted correctly. Uses getPixelR and getPixelG for
   // the gray and alpha channels respectively- this should
   // be changed to something more clearly labeled.

   {
      Image img("input/rgb.png");
      img.convertToGray(false);

      FatalIf(
            img.getFeatures() != 1,
            "Failed (Feature count). Expected 1, found %d.\n",
            img.getFeatures());

      FatalIf(
            img.getPixelR(0, 0) != Image::mRToGray,
            "Failed (Top Left). Expected %f, found %f.\n",
            (double)Image::mRToGray,
            (double)img.getPixelR(0, 0));

      FatalIf(
            img.getPixelR(1, 0) != Image::mGToGray,
            "Failed (Top Right). Expected %f, found %f.\n",
            (double)Image::mGToGray,
            (double)img.getPixelR(1, 0));

      FatalIf(
            img.getPixelR(0, 1) != Image::mBToGray,
            "Failed (Bottom Left). Expected %f, found %f.\n",
            (double)Image::mBToGray,
            (double)img.getPixelR(0, 1));

      FatalIf(
            img.getPixelR(1, 1) != 1.0f,
            "Failed (Bottom Right). Expected 1.0, found %f.\n",
            (double)img.getPixelR(1, 1));
   }

   {
      Image img("input/rgba.png");
      img.convertToGray(true);

      FatalIf(
            img.getFeatures() != 2,
            "Failed (Feature count). Expected 2, found %d.\n",
            img.getFeatures());

      FatalIf(
            img.getPixelR(0, 0) != Image::mRToGray,
            "Failed (Top Left). Expected %f, found %f.\n",
            (double)Image::mRToGray,
            (double)img.getPixelR(0, 0));

      FatalIf(
            img.getPixelR(1, 0) != Image::mGToGray,
            "Failed (Top Right). Expected %f, found %f.\n",
            (double)Image::mGToGray,
            (double)img.getPixelR(1, 0));

      FatalIf(
            img.getPixelR(0, 1) != Image::mBToGray,
            "Failed (Bottom Left). Expected %f, found %f.\n",
            (double)Image::mBToGray,
            (double)img.getPixelR(0, 1));

      FatalIf(
            img.getPixelG(1, 1) != 0.0f,
            "Failed (Bottom Right). Expected 0.0, found %f.\n",
            (double)img.getPixelG(1, 1));
   }
}

// Image::convertToColor(bool alphaChannel)
void testConvertToColor() {
   {
      Image img("input/rgb.png");
      img.convertToGray(false);
      img.convertToColor(false);

      FatalIf(
            img.getFeatures() != 3,
            "Failed (Feature count). Expected 3, found %d.\n",
            img.getFeatures());

      FatalIf(
            img.getPixelR(0, 0) != Image::mRToGray,
            "Failed (Top Left). Expected R = %f, found %f.\n",
            (double)Image::mRToGray,
            (double)img.getPixelR(0, 0));

      FatalIf(
            img.getPixelG(1, 0) != Image::mGToGray,
            "Failed (Top Right). Expected G = %f, found %f.\n",
            (double)Image::mGToGray,
            (double)img.getPixelG(1, 0));

      FatalIf(
            img.getPixelB(0, 1) != Image::mBToGray,
            "Failed (Bottom Left). Expected B = %f, found %f.\n",
            (double)Image::mBToGray,
            (double)img.getPixelB(0, 1));

      FatalIf(
            img.getPixelR(1, 1) != 1.0f || img.getPixelG(1, 1) != 1.0f
                  || img.getPixelB(1, 1) != 1.0f,
            "Failed (Bottom Right). Expected (1.0, 1.0, 1.0), found (%f, %f, %f).\n",
            (double)img.getPixelR(1, 1),
            (double)img.getPixelG(1, 1),
            (double)img.getPixelB(1, 1));
   }

   {
      Image img("input/rgba.png");
      img.convertToGray(true);
      img.convertToColor(true);
      FatalIf(
            img.getFeatures() != 4,
            "Failed (Feature count). Expected 4, found %d.\n",
            img.getFeatures());

      FatalIf(
            img.getPixelR(0, 0) != Image::mRToGray,
            "Failed (Top Left). Expected %f, found %f.\n",
            (double)Image::mRToGray,
            (double)img.getPixelR(0, 0));

      FatalIf(
            img.getPixelG(1, 0) != Image::mGToGray,
            "Failed (Top Right). Expected %f, found %f.\n",
            (double)Image::mGToGray,
            (double)img.getPixelG(1, 0));

      FatalIf(
            img.getPixelB(0, 1) != Image::mBToGray,
            "Failed (Bottom Left). Expected %f, found %f.\n",
            (double)Image::mBToGray,
            (double)img.getPixelB(0, 1));

      FatalIf(
            img.getPixelA(1, 1) != 0.0f,
            "Failed (Bottom Right). Expected 0.0, found %f.\n",
            (double)img.getPixelA(1, 1));
   }
}

int main(int argc, char **argv) {
   InfoLog() << "Testing Image::Image(png24): ";
   testPng24Load();
   InfoLog() << "Completed.\n";

   InfoLog() << "Testing Image::Image(png32): ";
   testPng32Load();
   InfoLog() << "Completed.\n";

   InfoLog() << "Testing Image::Image(jpg): ";
   testJpgLoad();
   InfoLog() << "Completed.\n";

   InfoLog() << "Testing Image::Image(bmp): ";
   testBmpLoad();
   InfoLog() << "Completed.\n";

   InfoLog() << "Testing Image::convertToGray(): ";
   testConvertToGray();
   InfoLog() << "Completed.\n";

   InfoLog() << "Testing Image::convertToColor(): ";
   testConvertToColor();
   InfoLog() << "Completed.\n";

   InfoLog() << "Image tests completed successfully!\n";
   return EXIT_SUCCESS;
}
