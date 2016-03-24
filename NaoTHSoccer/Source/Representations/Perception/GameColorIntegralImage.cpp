
#include "GameColorIntegralImage.h"
#include <assert.h>

void GameColorIntegralImage::shrinkBBPartPositive(GameColorIntegralImage::BoundingBox &bb, uint &dimension, int length, uint colorCount) const {
	const uint bound = std::abs((int)(dimension + length));
	while (length > 1) {
		length = (int)ceil(length / 2.0);
		if (dimension + length <= bound) {
			uint originalDimension = dimension;
			dimension += length;
			uint ccSub = getSumForRect(bb);
			if (ccSub != colorCount) {
				dimension = originalDimension;
			}
		}
	}
}

void GameColorIntegralImage::shrinkBBPartNegative(GameColorIntegralImage::BoundingBox &bb, uint &dimension, int length, uint colorCount) const {
	const uint bound = dimension - length;
	while (length > 1) {
		length = (int)ceil(length / 2.0);
		if (dimension - length >= bound) {
			uint originalDimension = dimension;
			dimension -= length;
			uint ccSub = getSumForRect(bb);
			if (ccSub != colorCount) {
				dimension = originalDimension;
			}
		}
	}
}

/*
void GameColorIntegralImage::shrinkBBPartPositive(GameColorIntegralImage::BoundingBox &bb, uint &dimension, int length, Color c1, Color c2, uint colorCount) const {
	const uint bound = std::abs(dimension + length);
	while (length > 1) {
		length = ceil(length / 2.0);
		if (dimension + length <= bound) {
			uint originalDimension = dimension;
			dimension += length;
			uint c1cSub = getSumForRect(bb, c1);
			uint c2cSub = getSumForRect(bb, c2);
			uint ccSub = c1cSub + c2cSub;
			if (ccSub != colorCount) {
				dimension = originalDimension;
			}
		}
	}
}

void GameColorIntegralImage::shrinkBBPartNegative(GameColorIntegralImage::BoundingBox &bb, uint &dimension, int length, Color c1, Color c2, uint colorCount) const {
	const uint bound = dimension - length;
	while (length > 1) {
		length = ceil(length / 2.0);
		if (dimension - length >= bound) {
			uint originalDimension = dimension;
			dimension -= length;
			uint c1cSub = getSumForRect(bb, c1);
			uint c2cSub = getSumForRect(bb, c2);
			uint ccSub = c1cSub + c2cSub;
			if (ccSub != colorCount) {
				dimension = originalDimension;
			}
		}
	}
}
*/
GameColorIntegralImage::BoundingBox GameColorIntegralImage::getShrunkBBInRect(BoundingBox bb) const {
	uint colorCnt = getSumForRect(bb);
//	assert(colorCnt > 0);

	shrinkBBPartPositive(bb, bb.minX, bb.getWidth(), colorCnt);
	shrinkBBPartPositive(bb, bb.minY, bb.getHeight(), colorCnt);

	shrinkBBPartNegative(bb, bb.maxX, bb.getWidth(), colorCnt);
	shrinkBBPartNegative(bb, bb.maxY, bb.getHeight(), colorCnt);

	return bb;
}

/*
GameColorIntegralImage::BoundingBox GameColorIntegralImage::getShrunkBBInRect(BoundingBox bb, Color c1, Color c2) const {
	uint color1Cnt = getSumForRect(bb, c1);
	uint color2Cnt = getSumForRect(bb, c2);
	uint colorCnt = color1Cnt + color2Cnt;
//	assert(colorCnt > 0);

	shrinkBBPartPositive(bb, bb.minX, bb.getWidth(), c1, c2, colorCnt);
	shrinkBBPartPositive(bb, bb.minY, bb.getHeight(), c1, c2, colorCnt);

	shrinkBBPartNegative(bb, bb.maxX, bb.getWidth(), c1, c2, colorCnt);
	shrinkBBPartNegative(bb, bb.maxY, bb.getHeight(), c1, c2, colorCnt);

	return bb;
}
*/
auto GameColorIntegralImage::getShrunkBBInRectInDirection(BoundingBox bb, ShrinkDirection _dir) const -> BoundingBox {
	uint colorCnt = getSumForRect(bb);

	if (hasShrinkDirection(_dir, Down)) {
		shrinkBBPartPositive(bb, bb.minY, bb.getHeight(), colorCnt);
	}
	if (hasShrinkDirection(_dir, Up)) {
		shrinkBBPartNegative(bb, bb.maxY, bb.getHeight(), colorCnt);
	}
	if (hasShrinkDirection(_dir, Right)) {
		shrinkBBPartPositive(bb, bb.minX, bb.getWidth(), colorCnt);
	}
	if (hasShrinkDirection(_dir, Left)) {
		shrinkBBPartNegative(bb, bb.maxX, bb.getWidth(), colorCnt);
	}

	return bb;
}

/*
auto GameColorIntegralImage::getGrowBBHorizontal(BoundingBox bb, ColorMix c, uint stepSize, double densityThreshold) const -> BoundingBox {
	int currentSum = getSumForRect(bb, c);
	densityThreshold = densityThreshold * stepSize;

	if (bb.minX > stepSize) {
		BoundingBox workBB = bb;
		workBB.minX -= stepSize;
		int workSum = getSumForRect(workBB, c);
		while ((workSum - currentSum) > bb.getHeight() * densityThreshold && workBB.minX > stepSize) {
			currentSum = workSum;
			bb = workBB;
			workBB.minX -= stepSize;
			workSum = getSumForRect(workBB, c);
		}
	}
	if (bb.maxX < width-stepSize-1) {
		BoundingBox workBB = bb;
		workBB.maxX += stepSize;
		int workSum = getSumForRect(workBB, c);
		while ((workSum - currentSum) > bb.getHeight() * densityThreshold && workBB.maxX < width-stepSize-1) {
			currentSum = workSum;
			bb = workBB;
			workBB.maxX += stepSize;
			workSum = getSumForRect(workBB, c);
		}
	}
	return bb;
}

auto GameColorIntegralImage::getGrowBBVertical(BoundingBox bb, ColorMix c, uint stepSize, double densityThreshold) const -> BoundingBox {
	int currentSum = getSumForRect(bb, c);
	densityThreshold = densityThreshold * stepSize;

	if (bb.minY > stepSize) {
		BoundingBox workBB = bb;
		workBB.minY -= stepSize;
		int workSum = getSumForRect(workBB, c);
		while ((workSum - currentSum) > bb.getWidth() * densityThreshold && workBB.minY > stepSize) {
			currentSum = workSum;
			bb = workBB;
			workBB.minY -= stepSize;
			workSum = getSumForRect(workBB, c);
		}
	}
	if (bb.maxY < height-1-stepSize) {
		BoundingBox workBB = bb;
		workBB.maxY += stepSize;
		int workSum = getSumForRect(workBB, c);
		while ((workSum - currentSum) > bb.getWidth() * densityThreshold && workBB.maxY < height-stepSize-1) {
			currentSum = workSum;
			bb = workBB;
			workBB.maxY += stepSize;
			workSum = getSumForRect(workBB, c);
		}
	}

	return bb;
}
*/
