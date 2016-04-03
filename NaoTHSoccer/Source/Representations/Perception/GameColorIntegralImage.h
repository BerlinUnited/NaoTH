#ifndef GAMECOLORINTEGRALIMAGE_H
#define GAMECOLORINTEGRALIMAGE_H

#include <stdint.h>
#include <algorithm>
#include <assert.h>
#include <sys/types.h>
#include <string.h>

#include <Tools/Math/Vector2.h>
#include <Tools/Debug/NaoTHAssert.h>

#ifdef WIN32
typedef unsigned int uint;
#endif

/**
 * Direction Down = shrink height from up to down
 *
 */
enum ShrinkDirection : uint8_t {
	Invalid = 0,
	Down    = 1,
	Up      = 2,
	Left    = 4,
	Right   = 8,
	All     = 15
};
inline ShrinkDirection operator|(ShrinkDirection d1, ShrinkDirection d2) {
	return ShrinkDirection(uint8_t(d1) | uint8_t(d2));
}
inline ShrinkDirection operator&(ShrinkDirection d1, ShrinkDirection d2) {
	return ShrinkDirection(uint8_t(d1) & uint8_t(d2));
}
inline bool hasShrinkDirection(ShrinkDirection d1, ShrinkDirection d2) {
	return (d1 & d2) != Invalid;
}


/**
 * @ingroup representations
 */
class GameColorIntegralImage {
public:
	class BoundingBox {
	public:

		enum NodeDirection : uint {
			NONE  = 0,
			NORTH = 1,
			EAST  = 2,
			SOUTH = 4,
			WEST  = 8,
		};

		uint minX, minY, maxX, maxY;

		BoundingBox() : minX(0), minY(0), maxX(0), maxY(0) {}
		BoundingBox(uint minX, uint minY, uint maxX, uint maxY)
			: minX(minX)
			, minY(minY)
			, maxX(maxX)
			, maxY(maxY) {}

		uint getArea() const {
			return (maxX - minX + 1) * (maxY - minY + 1);
		}

		uint getWidth() const {
			return maxX - minX + 1;
		}

		uint getHeight() const {
			return maxY - minY + 1;
		}

		/**
		 * where does bb touch this BoundingBox?
		 * @param bb
		 * @return
		 */
		NodeDirection isAtBorder(BoundingBox const& bb) const {
			uint ret = uint(NONE);
			if (minX == bb.minX ||
				minX == bb.maxX) {
				ret |= uint(WEST);
			}
			if (minY == bb.minY ||
				minY == bb.maxY) {
				ret |= uint(NORTH);
			}
			if (maxX == bb.maxX ||
				maxX == bb.minX) {
				ret |= uint(EAST);
			}
			if (maxY == bb.maxY ||
				maxY == bb.minY) {
				ret |= uint(SOUTH);
			}
			return NodeDirection(ret);
		}

		bool containsPoint(uint x, uint y) const {
			return (x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY);
		}

		bool overlapsWithBB(BoundingBox const& bb) const {
			bool ret = false;
			ret |= containsPoint(bb.minX, bb.minY);
			ret |= containsPoint(bb.minX, bb.maxY);
			ret |= containsPoint(bb.maxX, bb.minY);
			ret |= containsPoint(bb.maxX, bb.maxY);

			ret |= bb.containsPoint(minX, minY);
			ret |= bb.containsPoint(minX, maxY);
			ret |= bb.containsPoint(maxX, minY);
			ret |= bb.containsPoint(maxX, maxY);
			return ret;
		}

		bool touchesWithBB(BoundingBox const &bb) const {
			if (minX == bb.minX ||
				minX == bb.maxX ||
				maxX == bb.maxX ||
				maxX == bb.minX)
			{
				return (bb.minY <= minY && minY <= bb.maxY) ||
						(bb.minY <= maxY && maxY <= bb.maxY) ||
						(minY <= bb.minY && bb.minY <= maxY) ||
						(minY <= bb.maxY && bb.maxY <= maxY);
			} else if (minY == bb.minY ||
						minY == bb.maxY ||
						maxY == bb.maxY ||
						maxY == bb.minY) {
				return (bb.minX <= minX && minX <= bb.maxX) ||
						(bb.minX <= maxX && maxX <= bb.maxX) ||
						(minX <= bb.minX && bb.minX <= maxX) ||
						(minX <= bb.maxX && bb.maxX <= maxX);
			}
			return false;
		}

		BoundingBox merge(BoundingBox const& bb) const {
			BoundingBox ret;
			ret.minX = std::min(minX, bb.minX);
			ret.minY = std::min(minY, bb.minY);
			ret.maxX = std::max(maxX, bb.maxX);
			ret.maxY = std::max(maxY, bb.maxY);
			return ret;
		}

		Vector2d getCenterPoint(void) const {
			return Vector2d((minX + maxX) / 2, (minY + maxY) / 2);
		}

		std::string toString() const {
			//std::string ret = "minX: " + std::to_string(minX) + ", minY: " + std::to_string(minY) + ", maxX: " + std::to_string(maxX) + ", maxY: " + std::to_string(maxY);
			return "";//ret;
		}
	};

  GameColorIntegralImage()
		: integralImage(nullptr)
		, width(0)
		, height(0) 
  {
	}
	~GameColorIntegralImage() {
		delete[] integralImage;
	}

	void setDimension(uint32_t _width, uint32_t _height) {
		if (height != _height || width != _width) {
			delete[] integralImage;
			width = _width;
			height = _height;
			integralImage = new uint32_t[height*width*MAX_COLOR];
			memset(integralImage, 0, height*width*MAX_COLOR*sizeof(uint32_t));
		}
	}

	bool isValid() const {
		return integralImage != nullptr;
	}

	uint32_t* getDataPointer() {
		return integralImage;
	}

	uint getSumForRect(const BoundingBox& bb, uint32_t c) const {
    return getSumForRect(bb.minX, bb.minY, bb.maxX, bb.maxY, c);
  }

  uint getSumForRect(uint minX, uint minY, uint maxX, uint maxY, uint32_t c) const {
		ASSERT(minX > 0);
    ASSERT(minY > 0);
    
    minY -= 1;
    minX -= 1;
    //maxX = maxX + 1;
		//maxY = maxY + 1;

		ASSERT(maxX < width);
		ASSERT(maxY < height);

		const uint32_t idx1 = (minY * width + minX)*MAX_COLOR;
		const uint32_t idx2 = (minY * width + maxX)*MAX_COLOR;
		const uint32_t idx3 = (maxY * width + minX)*MAX_COLOR;
		const uint32_t idx4 = (maxY * width + maxX)*MAX_COLOR;

		ASSERT(nullptr != integralImage);
		const uint32_t *buffer1 = integralImage + idx1;
		const uint32_t *buffer2 = integralImage + idx2;
		const uint32_t *buffer3 = integralImage + idx3;
		const uint32_t *buffer4 = integralImage + idx4;

		const uint32_t sum = buffer1[c] + buffer4[c] - buffer2[c] - buffer3[c];
		return sum;
	}

  double getDensityForRect(uint minX, uint minY, uint maxX, uint maxY, uint32_t c) const {
		uint count = getSumForRect(minX, minY, maxX, maxY, c);
		return double(count) / double((maxX-minX)*(maxY-minY));
	}

	double getDensityForRect(const BoundingBox &bb, uint32_t c) const {
		uint count = getSumForRect(bb, c);
		return double(count) / double(bb.getArea());
	}

	uint32_t getWidth() const {
		return width;
	}

	uint32_t getHeight() const {
		return height;
	}

	/**
	 * height and width of the "original" image
	 * (less one row and col)
	 * @return
	 */
	uint32_t getImgWidth() const {
		return width - 1;
	}

	uint32_t getImgHeight() const {
		return height - 1;
	}

	BoundingBox getBoundingBox() const {
		return BoundingBox(0, 0, width - 2, height - 2);
	}

	BoundingBox getShrunkBBInRect(BoundingBox bb, uint32_t c) const;

	//BoundingBox getShrunkBBInRect(BoundingBox bb, Color c1, Color c2) const;

	BoundingBox getShrunkBBInRectInDirection(BoundingBox bb, ShrinkDirection direction, uint32_t c) const;

	//BoundingBox getGrowBBHorizontal(BoundingBox bb, ColorMix c, uint stepSize, double densityThreshold=0.5) const;
	//BoundingBox getGrowBBVertical  (BoundingBox bb, ColorMix c, uint stepSize, double densityThreshold=0.5) const;

public:
  static const uint32_t MAX_COLOR = 3;
  static const int32_t FACTOR = 4;

private:
	uint32_t* integralImage;
	uint32_t width, height;

	void shrinkBBPartPositive(BoundingBox &bb, uint &dimension, int length, uint32_t c, uint colorCount) const;
	void shrinkBBPartNegative(BoundingBox &bb, uint &dimension, int length, uint32_t c, uint colorCount) const;

//	void shrinkBBPartPositive(BoundingBox &bb, uint &dimension, int length, Color c1, Color c2, uint colorCount) const;
//	void shrinkBBPartNegative(BoundingBox &bb, uint &dimension, int length, Color c1, Color c2, uint colorCount) const;
};

class GameColorIntegralImageTop : public GameColorIntegralImage 
{
};


inline GameColorIntegralImage::BoundingBox::NodeDirection operator&(GameColorIntegralImage::BoundingBox::NodeDirection bb1, GameColorIntegralImage::BoundingBox::NodeDirection bb2) {
	return GameColorIntegralImage::BoundingBox::NodeDirection(uint(bb1) & uint(bb2));
}

inline GameColorIntegralImage::BoundingBox::NodeDirection operator|(GameColorIntegralImage::BoundingBox::NodeDirection bb1, GameColorIntegralImage::BoundingBox::NodeDirection bb2) {
	return GameColorIntegralImage::BoundingBox::NodeDirection(uint(bb1) | uint(bb2));
}

inline GameColorIntegralImage::BoundingBox::NodeDirection operator&=(GameColorIntegralImage::BoundingBox::NodeDirection& bb1, GameColorIntegralImage::BoundingBox::NodeDirection bb2) {
	return bb1 = bb1 & bb2;
}

inline GameColorIntegralImage::BoundingBox::NodeDirection operator|=(GameColorIntegralImage::BoundingBox::NodeDirection& bb1, GameColorIntegralImage::BoundingBox::NodeDirection bb2) {
	return bb1 = bb1 | bb2;
}

#endif
