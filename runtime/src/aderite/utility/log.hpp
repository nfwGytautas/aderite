#pragma once

#include <memory>
#include "spdlog/spdlog.h"

namespace aderite {

	/**
	 * @brief Aderite spdlog interface
	*/
	class logger {
	public:
		/**
		 * @brief Returns the logger instance
		*/
		static logger* get();

		/**
		 * @brief Initializes logger, done automatically by the runtime
		*/
		void init();

		// START OF SPDLOG INTERFACE

		template<typename... Args>
		void debug(Args &&...args)
		{
			m_logger->debug(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void trace(Args &&...args)
		{
			m_logger->trace(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void info(Args &&...args)
		{
			m_logger->info(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void warn(Args &&...args)
		{
			m_logger->warn(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void error(Args &&...args)
		{
			m_logger->error(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void critical(Args &&...args)
		{
			m_logger->critical(std::forward<Args>(args)...);
		}

		// END OF SPDLOG INTERFACE
	private:
		logger() {}
		logger(const logger& o) = delete;

		std::shared_ptr<spdlog::logger> m_logger;
	};

}

// Endpoint macros

// BLUE
#define LOG_DEBUG(...)       ::aderite::logger::get()->debug(__VA_ARGS__)

// WHITE
#define LOG_TRACE(...)       ::aderite::logger::get()->trace(__VA_ARGS__)

// GREEN
#define LOG_INFO(...)        ::aderite::logger::get()->info(__VA_ARGS__)

// YELLOW
#define LOG_WARN(...)        ::aderite::logger::get()->warn(__VA_ARGS__)

// RED
#define LOG_ERROR(...)       ::aderite::logger::get()->error(__VA_ARGS__)

// FATAL
#define LOG_FATAL(...)		 ::aderite::logger::get()->critical(__VA_ARGS__)
