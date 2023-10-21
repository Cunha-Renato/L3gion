#pragma once

namespace L3gion
{
	class Timestep
	{
	public:
		Timestep(double time = 0.0f)
			: m_Time(time)
		{
		}

		operator double() const { return m_Time; }

		double getSeconds() const { return m_Time; }
		double getMiliSeconds() const { return m_Time * 1000.0f; }
	private:
		double m_Time;
	};
}