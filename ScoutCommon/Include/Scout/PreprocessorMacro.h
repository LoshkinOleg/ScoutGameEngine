#pragma once

#define SCOUT_NO_COPY(typeName) typeName(const typeName&) = delete; typeName& operator=(const typeName&) = delete;
#define SCOUT_NO_MOVE(typeName) typeName(typeName&&) = delete; typeName& operator=(typeName&&) = delete;
#define SCOUT_DECLARE_HANDLE(handleName) struct handleName{ inline handleName(const std::uint64_t id): id(id){} inline operator std::uint64_t() const {return id;} std::uint64_t id = 0; constexpr static const std::uint64_t INVALID_ID = (std::uint64_t)-1; };

// TODO: macros to throw exceptions in a more readable manner
// SCOUT_THROW(...)