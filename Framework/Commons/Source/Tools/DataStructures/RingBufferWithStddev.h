#ifndef RINGBUFFERWITHSTDDEV_H
#define RINGBUFFERWITHSTDDEV_H

#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Common.h"

/**
 * Ringbuffer whiich can calcuate the standard deviation and variance of the contained data.
 * It can calculate the mean and median of the stored data. In addition
 * it can calculcate the variance and standard deviation with the excpected
 * value of mean OR median.
 */
template <class C, int n=100> class RingBufferWithStddev : public RingBufferWithSum<C, n> {
public:
    /**
     * @brief the expected values available for calculating variance and/or standard deviation.
     */
    enum E { mean, median };

    /**
     * @brief Determines and calculates the median.
     *        The only difference compared to RingBufferWithSum is on even sized buffers.
     *        There, the median is calculated as mean between the two median values.
     * @return the median value of all values
     */
    C getMedian() const {
        // Return 0 if buffer is empty
        if (0==this->numberOfEntries) return C();
        // copy buffer
        C temp[n];
        memcpy(temp, this->buffer, this->numberOfEntries * sizeof(C));
        int mid = this->numberOfEntries/2;
        // check for median idx (even / odd)
        if(this->numberOfEntries % 2) {
            std::nth_element(temp, temp+mid, temp+this->numberOfEntries);
            return temp[mid];
        } else {
            std::nth_element(temp, temp+(mid-1), temp+this->numberOfEntries);
            C t1 = temp[mid-1];

            std::nth_element(temp, temp+mid, temp+this->numberOfEntries);
            C t2 = temp[mid];
            // take the avg of the two middle elements
            return (t1 + t2) / 2.0;
        }
        // NOTE: instead of sorting the whole array, we're just searching for the middle item
        //       even for odd elements it is faster!
    }

    /**
     * @brief Calculates the variance and uses therefore the defined expected value (mean, median, ...).
     *        By default the mean is used as expected value.
     * @param e the expected value, which should be used
     * @return the calculated variance
     */
    C getVariance(E e = mean) const {
        return getVariance(this->e(e));
    }

    /**
     * @brief Calculates the variance and uses therefore the given expected value.
     * @param e the expected value, which should be used
     * @return the calculated variance
     */
    C getVariance(C e) const {
        C temp = 0;
        for (int i = 0; i < this->numberOfEntries; ++i) {
            temp += Math::sqr(this->buffer[i]-e);
        }
        return temp/this->numberOfEntries;
    }

    /**
     * @brief Calculates the standard deviation and uses the defined expected value.
     *        By default the mean is used as expected value.
     * @param e the expected value, which should be used
     * @return the caluclated standard deviation
     */
    C getStdDev(E e = mean) const {
        return std::sqrt(this->getVariance(e));
    }

    /**
     * @brief Calculates the standard deviation and uses the given expected value.
     * @param e the expected value, which should be used
     * @return the caluclated standard deviation
     */
    C getStdDev(C e) const {
        return std::sqrt(this->getVariance(e));
    }

    /**
     * @brief Calculated the adjusted mean of all values.
     *        Only values one standard deviation from the median are used.
     *        And only the upper bound is applied!
     * @return the adjusted mean
     */
    C getAdjustedAverage() const {
        C sum = 0;
        int count = 0;
        C max = getMedian() + getStdDev(median);
        for (int i = 0; i < this->numberOfEntries; ++i) {
            if(this->buffer[i] > max) {continue;}
            sum += this->buffer[i];
            count++;
        }
        return sum/count;
    }

private:
    /**
     * @brief Determines the expected value.
     *        Helper method used to get the "right" expected value in variance/standard deviation.
     * @param v the requested expected value
     * @return the expected value (median, mean, ...)
     */
    inline C e(E v) const {
        switch (v) {
        case median:
            return this->getMedian();
            break;
        case mean:
        default:
            return this->getAverage();
        }
    }
};

#endif // RINGBUFFERWITHSTDDEV_H
