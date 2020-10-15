#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H

#include <cassert>
#include <memory>


class Timer
{
public:
    Timer() {}
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    virtual ~Timer() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Update() = 0;
};

namespace TimerImplDetails
{
    template<typename TimePeriodType,
            typename TimePointType,
            typename GetTimeFuncType,
            typename FuncType,
            bool SingleShot>
    class TimerImpl;


    template<typename TimePeriodType,
            typename TimePointType,
            typename GetTimeFuncType,
            typename FuncType>
    class TimerImpl<TimePeriodType, TimePointType, GetTimeFuncType, FuncType, false> : public Timer
    {
    public:
        explicit TimerImpl(GetTimeFuncType&& getTimeFunc, const TimePeriodType& period, FuncType&& func)
                : stopped_(true)
                , lastUpdateTime_()
                , period_(period)
                , whatsATime_(std::forward<GetTimeFuncType>(getTimeFunc))
                , func_(std::forward<FuncType>(func))
        {
            assert(period > TimePeriodType(0));
        }

        void Start() override
        {
            stopped_ = false;
            lastUpdateTime_ = whatsATime_();
        }
        void Stop() override { stopped_ = true; }

        void Update() override
        {
            if(stopped_ || (whatsATime_() - lastUpdateTime_) < period_)
                return;

            func_();
            lastUpdateTime_ = whatsATime_();
        }

    private:
        bool stopped_;
        TimePointType lastUpdateTime_;
        TimePeriodType period_;
        GetTimeFuncType whatsATime_;
        FuncType func_;
    };

    template<typename TimePeriodType,
            typename TimePointType,
            typename GetTimeFuncType,
            typename FuncType>
    class TimerImpl<TimePeriodType, TimePointType, GetTimeFuncType, FuncType, true> : public Timer
    {
    public:
        explicit TimerImpl(GetTimeFuncType&& getTimeFunc, const TimePeriodType& period, FuncType&& func)
                : stopped_(true)
                , fired_(false)
                , lastUpdateTime_()
                , period_(period)
                , whatsATime_(std::forward<GetTimeFuncType>(getTimeFunc))
                , func_(std::forward<FuncType>(func))
        {
        }

        void Start() override
        {
            stopped_ = false;
            lastUpdateTime_ = whatsATime_();
        }
        void Stop() override
        {
            stopped_ = true;
        }

        void Update() override
        {
            if(fired_ || stopped_ || (whatsATime_() - lastUpdateTime_) < period_)
                return;

            func_();
            fired_ = true;
        }

    private:
        bool stopped_;
        bool fired_;
        TimePointType lastUpdateTime_;
        TimePeriodType period_;
        GetTimeFuncType whatsATime_;
        FuncType func_;
    };
}



template<typename TimePeriodType, typename GetTimeFuncType, typename FuncType>
std::unique_ptr<Timer> MakeTimer(GetTimeFuncType&& whatsATime, const TimePeriodType& period, FuncType&& func)
{
    using namespace TimerImplDetails;
    return std::unique_ptr<Timer>(new TimerImpl<TimePeriodType, decltype(std::declval<GetTimeFuncType>()()), GetTimeFuncType, FuncType, false>(
            std::forward<GetTimeFuncType>(whatsATime),
            period,
            std::forward<FuncType>(func)));
}

template<typename TimePeriodType, typename GetTimeFuncType, typename FuncType>
std::unique_ptr<Timer> MakeSingleShotTimer(GetTimeFuncType&& whatsATime, const TimePeriodType& period, FuncType&& func)
{
    using namespace TimerImplDetails;
    return std::unique_ptr<Timer>(new TimerImpl<TimePeriodType, decltype(std::declval<GetTimeFuncType>()()), GetTimeFuncType, FuncType, true>(
            std::forward<GetTimeFuncType>(whatsATime),
            period,
            std::forward<FuncType>(func)));
}


#endif //TIMER_TIMER_H
