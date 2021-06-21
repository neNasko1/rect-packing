#include "TestingUtils.h"

#include <time.h>
#include <math.h>
#include <fstream>

#include "../RectanglePacker/RectanglePacker.h"
#include "../Shapes/Shapes.h"

namespace rectpack {
namespace testinglib {
    float toRad(float deg) {
        return deg * 3.14159265 / 180.;
    }

    Box boundingBox(const Box &box) {
        if(std::fabs(box.angle) <= 1e-5) {
            return box;
        } else if (std::fabs(box.angle - 90) <= 1e-4) {
            return Box(box.x - box.height, box.y, box.height, box.width);
        }
        const float PI = 3.14159265;
        float transformWidthxDelta = (float)box.width * cos(toRad(box.angle)), transformWidthyDelta = (float)box.width * sin(toRad(box.angle));
        float transformHeightxDelta = (float)box.height * cos(toRad(box.angle) + PI / 2.), transformHeightyDelta = (float)box.height * sin(toRad(box.angle) + PI / 2.);

        // Remove 2 pixels from the borders for more accurate checking.
        cordType AABBleftx  = (std::min(std::min(0.f, transformWidthxDelta), std::min(transformHeightxDelta, transformWidthxDelta + transformHeightxDelta)) + 0.5);
        cordType AABBrightx = (std::max(std::max(0.f, transformWidthxDelta), std::max(transformHeightxDelta, transformWidthxDelta + transformHeightxDelta)) + 0.5);
        cordType AABBdowny  = (std::min(std::min(0.f, transformWidthyDelta), std::min(transformHeightyDelta, transformWidthyDelta + transformHeightyDelta)) + 0.5);
        cordType AABBupy    = (std::max(std::max(0.f, transformWidthyDelta), std::max(transformHeightyDelta, transformWidthyDelta + transformHeightyDelta)) + 0.5);
        return Box(AABBleftx + box.x, AABBdowny + box.y, AABBrightx - AABBleftx - 2, AABBupy - AABBdowny - 2);
    }

    bool boundingBoxAreCollidingAABB(const Box &a, const Box &b) {
        auto bba = boundingBox(a), bbb = boundingBox(b);
        if(areCollidingAABB(bba, bbb)) {
            std::cout << a << " " << b << std::endl;
            std::cout << bba << " " << bbb << " " << areCollidingAABB(bba, bbb) << std::endl;
        }
        return areCollidingAABB(bba, bbb);
    }

    bool okPacking(const Packing &packed) {
        for(int i = 0; (size_t)i < packed.shapes.size(); i ++) {
            for(int j = i + 1; (size_t)j < packed.shapes.size(); j ++) {
                if(boundingBoxAreCollidingAABB(packed.shapes[i], packed.shapes[j])) {
                    std::cout << packed.shapes[i] << " " << packed.shapes[j] << std::endl;
                    return true;
                }
            }
            if(!Box(0, 0, 200, 200, 0).containsAABB(boundingBox(packed.shapes[i]))) {
                return false;
            }
        }
        return false;
    }

    void stressTest(const float maxTime) {
        auto beginClock = clock();
        int cnt = 0;
        while(clock() - beginClock < maxTime) {
            cnt ++;
            RectanglePacker rectPackerToTest(Rectangle(200, 200), rand() % 8, 3000, cnt);
            std::vector<std::pair<Rectangle, int> > currentInput;
            for(int i = 0; i < 10; i ++) {
                int w = rand() % 20 + 10, h = rand() % 20 + 10;
                currentInput.push_back({Rectangle(w, h), 30});
                for(int j = 0; j < 30; j ++) {
                    rectPackerToTest.shapes.push_back(Rectangle(w, h));
                }
            }
            rectPackerToTest.execute();
            std::ofstream outstream; outstream.open("rect.svg");
            rectPackerToTest.outputToSvg(outstream);
            outstream.close();
            if(okPacking(rectPackerToTest.packed)) {
                std::cout << "Test was not passed " << std::endl;
                std::cout << "Bin 200 200; Seed " << cnt << "; Mask " << rectPackerToTest.mask << std::endl;
                for(const auto &it : currentInput) {
                    std::cout << "{\"W\":" << it.first.width << ", \"H\":" << it.first.height << ", \"Count\":" << it.second << "}, ";
                }
                return;
            } else {
                std::cout << "Test passed " << rectPackerToTest.mask << " " << rectPackerToTest.packed.score  << std::endl;
            }
        }
    }
}
}
