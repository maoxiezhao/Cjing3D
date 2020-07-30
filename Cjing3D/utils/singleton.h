#pragma once

namespace Cjing3D
{
	template<typename T>
	class Singleton
	{
	public:
		~Singleton() = default;
		Singleton(const Singleton<T>& rhs) = delete;
		Singleton(Singleton<T>&& rhs) = delete;
		Singleton& operator=(const Singleton<T>& rhs) = delete;
		Singleton& operator=(Singleton<T>&& rhs) = delete;

		T& GetInstance()
		{
			static T instance;
			return instance;
		}

	protected:
		Singleton() = default;

	};
}