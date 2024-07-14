
#include <functional>

/* \brief Executing update will accumulate time, if the time specified by Timer is exceeded,
 *  the function OnTimerEnd will be executed.
 */
class TimerTrigger
{
public:
    TimerTrigger() = delete;

    TimerTrigger(float timer, std::function<void(float deltaTime)>& OnTimerEndReset) :
        mTimer(timer), mTime(0.0f), mOnTimerEndReset(OnTimerEndReset) {}

    /* \brief Returns true if the function was triggered. */
    bool update(float deltaTime)
    {
        mTime += deltaTime;

        if (mTime > mTimer)
        {
            // mTime considers the small variations from mTimer so it should be more accurate.
            mOnTimerEndReset(mTime);
            mTime -= mTimer;

            return true;
        }
        return false;
    }

private:
    float mTimer;
    float mTime;
    std::function<void(float deltaTime)> mOnTimerEndReset;
};