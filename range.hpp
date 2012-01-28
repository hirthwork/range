#ifndef __RANGE_HPP_2012_01_28__
#define __RANGE_HPP_2012_01_28__

#include <reinvented-wheels/enableif.hpp>

#include "emptyassert.hpp"
#include "predicates.hpp"

namespace NRange
{
    struct TSingleValueTag
    {
    };

    template <class TType, class TAssert = TEmptyAssert,
        class TTag = TSingleValueTag>
    class TSingleValue
    {
        TType Value_;
        bool Empty_;

    public:
        typedef TType TType_;
        typedef TAssert TAssert_;
        typedef TTag TRangeTag_;

        inline explicit TSingleValue(const TType& value)
            : Value_(value)
            , Empty_(false)
        {
        }

        inline bool IsEmpty() const
        {
            return Empty_;
        }

        inline void Pop()
        {
            TAssert_::Assert(Not(TSame(Empty_)),
                "TSingleValue::Pop called twice");
            Empty_ = true;
        }

        inline const TType_& Front() const
        {
            TAssert_::Assert(Not(TSame(Empty_)),
                "TSingleValue::Front called after Pop");
            return Value_;
        }
    };

    struct TRepeatedRangeTag
    {
    };

    template <class TRange, class TCounter, class TAssert = TEmptyAssert,
        class TTag = TRepeatedRangeTag>
    class TRepeatedRange
    {
        TRange Range_;
        TRange CurrentRange_;
        TCounter Counter_;

    public:
        typedef typename TRange::TType_ TType_;
        typedef TAssert TAssert_;
        typedef TTag TRangeTag_;

        inline TRepeatedRange(const TRange& range,
            const TCounter& counter)
            : Range_(range)
            , CurrentRange_(Range_)
            , Counter_(counter)
        {
            --Counter_;
        }

        inline bool IsEmpty() const
        {
            return CurrentRange_.IsEmpty() && (!Counter_ || Range_.IsEmpty());
        }

        inline void Pop()
        {
            TAssert_::Assert(Not(BindMember(&CurrentRange_, &TRange::IsEmpty)),
                "TRepeatedRange::Pop called on empty range");
            CurrentRange_.Pop();
            if (CurrentRange_.IsEmpty() && !!Counter_)
            {
                --Counter_;
                CurrentRange_ = Range_;
            }
        }

        inline const TType_& Front() const
        {
            TAssert_::Assert(Not(BindMember(&CurrentRange_, &TRange::IsEmpty)),
                "TRepeatedRange::Front called on empty range");
            return CurrentRange_.Front();
        }
    };

    struct TComposedRangeTag
    {
    };

    template <class TRange1, class TRange2, class TAssert = TEmptyAssert,
        class TTag = TComposedRangeTag>
    class TComposedRange
    {
        TRange1 Range1_;
        TRange2 Range2_;

    public:
        typedef typename TRange1::TType_ TType_;
        typedef TAssert TAssert_;
        typedef TTag TRangeTag_;

        inline TComposedRange(TRange1 range1, TRange2 range2)
            : Range1_(range1)
            , Range2_(range2)
        {
        }

        inline bool IsEmpty() const
        {
            return Range1_.IsEmpty() && Range2_.IsEmpty();
        }

        inline void Pop()
        {
            TAssert_::Assert(BindMember(this, &TComposedRange::IsEmpty),
                "TComposedRange::Pop called on empty range");
            if (Range1_.IsEmpty())
            {
                Range2_.Pop();
            }
            else
            {
                Range1_.Pop();
            }
        }

        inline const TType_& Front() const
        {
            TAssert_::Assert(BindMember(this, &TComposedRange::IsEmpty),
                "TComposedRange::Front called on empty range");
            if (Range1_.IsEmpty())
            {
                return Range2_.Front();
            }
            else
            {
                return Range1_.Front();
            }
        }
    };

    struct TInfiniteCounter
    {
        bool operator !() const
        {
            return false;
        }

        TInfiniteCounter& operator --()
        {
            return *this;
        }
    };

    template <class TType>
    static inline TSingleValue<TType> SingleValue(const TType& value)
    {
        return TSingleValue<TType>(value);
    }

    template <class TAssert, class TType>
    static inline TSingleValue<TType, TAssert> SingleValue(const TType& value)
    {
        return TSingleValue<TType, TAssert>(value);
    }

    template <class TRange, class TCounter>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange::TRangeTag_),
        TRepeatedRange<TRange, TCounter, typename TRange::TAssert_>
        >::TType_ operator *(const TRange& range, const TCounter& counter)
    {
        return TRepeatedRange<TRange, TCounter, typename TRange::TAssert_>(
            range, counter);
    }

    template <class TAssert, class TRange, class TCounter>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange::TRangeTag_),
        TRepeatedRange<TRange, TCounter, TAssert>
        >::TType_ operator *(const TRange& range, const TCounter& counter)
    {
        return TRepeatedRange<TRange, TCounter, TAssert>(range, counter);
    }

    template <class TRange1, class TRange2>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange1::TRangeTag_)
        && sizeof(typename TRange2::TRangeTag_),
        TComposedRange<TRange1, TRange2, typename TRange1::TAssert_>
        >::TType_ ComposeRange(const TRange1& range1, const TRange2& range2)
    {
        return TComposedRange<TRange1, TRange2, typename TRange1::TAssert_>(
            range1, range2);
    }

    template <class TAssert, class TRange1, class TRange2>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange1::TRangeTag_)
        && sizeof(typename TRange2::TRangeTag_),
        TComposedRange<TRange1, TRange2, TAssert>
        >::TType_ ComposeRange(const TRange1& range1, const TRange2& range2)
    {
        return TComposedRange<TRange1, TRange2, TAssert>(range1, range2);
    }

    template <class TRange1, class TRange2, class TRange3>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange1::TRangeTag_)
        && sizeof(typename TRange2::TRangeTag_)
        && sizeof(typename TRange3::TRangeTag_),
        TComposedRange<
            TComposedRange<TRange1, TRange2, typename TRange1::TAssert_>,
            TRange3, typename TRange1::TAssert_>
        >::TType_ ComposeRange(const TRange1& range1, const TRange2& range2,
            const TRange3& range3)
    {
        return TComposedRange<
            TComposedRange<TRange1, TRange2, typename TRange1::TAssert_>,
            TRange3, typename TRange1::TAssert_>(ComposeRange(range1, range2),
                range3);
    }

    template <class TAssert, class TRange1, class TRange2, class TRange3>
    static inline typename NReinventedWheels::TEnableIf<
        sizeof(typename TRange1::TRangeTag_)
        && sizeof(typename TRange2::TRangeTag_)
        && sizeof(typename TRange3::TRangeTag_),
        TComposedRange<TComposedRange<TRange1, TRange2, TAssert>, TRange3,
            TAssert>
        >::TType_ ComposeRange(const TRange1& range1, const TRange2& range2,
            const TRange3& range3)
    {
        return TComposedRange<TComposedRange<TRange1, TRange2, TAssert>,
            TRange3, TAssert>(ComposeRange<TAssert>(range1, range2), range3);
    }

    template <class TType>
    static inline TComposedRange<TSingleValue<TType>, TSingleValue<TType> >
        SingleValuesList(const TType& value1, const TType& value2)
    {
        return ComposeRange(SingleValue(value1), SingleValue(value2));
    }

    template <class TAssert, class TType>
    static inline TComposedRange<TSingleValue<TType>, TSingleValue<TType>,
        TAssert> SingleValuesList(const TType& value1, const TType& value2)
    {
        return ComposeRange<TAssert>(SingleValue(value1), SingleValue(value2));
    }

    template <class TType>
    static inline TComposedRange<
        TComposedRange<TSingleValue<TType>, TSingleValue<TType> >,
        TSingleValue<TType> >
        SingleValuesList(const TType& value1, const TType& value2,
        const TType& value3)
    {
        return ComposeRange(SingleValue(value1), SingleValue(value2),
            SingleValue(value3));
    }

    template <class TAssert, class TType>
    static inline TComposedRange<
        TComposedRange<TSingleValue<TType>, TSingleValue<TType>, TAssert>,
        TSingleValue<TType>, TAssert>
        SingleValuesList(const TType& value1, const TType& value2,
        const TType& value3)
    {
        return ComposeRange<TAssert>(SingleValue(value1), SingleValue(value2),
            SingleValue(value3));
    }
}

#endif

