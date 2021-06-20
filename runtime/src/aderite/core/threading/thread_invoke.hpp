#pragma once

#include <mutex>
#include <condition_variable>
#include <functional>

namespace aderite {
	namespace thread {

		/**
		 * @brief Base invoke task for threads
		*/
		class thread_invoke_base {
		public:
			virtual ~thread_invoke_base() {}

			/**
			 * @brief Function that is executed
			*/
			virtual void call() = 0;

			/**
			 * @brief Blocks until the invoke is finished
			*/
			void wait();

			/**
			 * @brief Returns the result of the invoke
			*/
			virtual void* result() = 0;
		protected:
			bool m_finished = false;
			std::mutex m_mutex;
			std::condition_variable m_done;
		};

		/**
		 * @brief std::function invoke
		*/
		template<typename T>
		class f_invoke : public thread_invoke_base {
		public:
			f_invoke(std::function<T(void)> f)
				: m_f(std::move(f))
			{}

			virtual void call() override {
				m_result = m_f();
				m_finished = true;
				m_done.notify_all();
			}

			virtual void* result() {
				return static_cast<void*>(&m_result);
			}

		private:
			std::function<T(void)> m_f;
			T m_result;
		};

		/**
		 * @brief std::function void invoke
		*/
		class void_invoke : public thread_invoke_base {
		public:
			void_invoke(std::function<void(void)> f)
				: m_f(std::move(f))
			{}

			virtual void call() override {
				m_f();
				m_finished = true;
				m_done.notify_all();
			}

			virtual void* result() {
				return nullptr;
			}

		private:
			std::function<void(void)> m_f;
		};

	}
}
