#pragma once
#include <chrono>

class Timer
{
public:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1>>;
	using typeTimeStamp = std::chrono::time_point<clock_t>;

public:
	static Timer* GetTimer()
	{
		static Timer* timer = new Timer();
		return timer;
	}

	double GetEllapsedSeconds() const noexcept
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - timeStamp).count();
	}

	double GetCurrentTime() const noexcept
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - startTimeStamp).count();
	}

	// Have to call only one time at each frame
	double GetDeltaTime()
	{
		double dt = GetEllapsedSeconds();
		fpsEllapsedTime += dt;
		fpsFrame++;

		Reset();

		return dt;
	}

	// It returns -1 if ellapsed time is less than a second.
	int GetFPSFrame()
	{
		int frame = -1;

		if (fpsEllapsedTime >= 1.f)
		{
			fpsEllapsedTime = 0.f;
			frame = fpsFrame;
			fpsFrame = 0;
		}

		return frame;
	}

private:
	Timer() : timeStamp(clock_t::now()), startTimeStamp(timeStamp), fpsEllapsedTime(0.f), fpsFrame(0)
	{}

	void Reset() noexcept
	{
		timeStamp = clock_t::now();
	}

	typeTimeStamp timeStamp;
	typeTimeStamp startTimeStamp;

	double fpsEllapsedTime;
	int fpsFrame;
};