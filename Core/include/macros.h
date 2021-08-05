#pragma once

#ifndef sgeGuard_InstanciationRules
#define sgeGuard_InstanciationRules
#define sge_DISALLOW_COPY(type) type(type&) = delete; type& operator=(type&) = delete;
#define sge_DISALLOW_CONSTRUCTION(type) type() = delete; ~type() = delete;
#define sge_ALLOW_CONSTRUCTION(type) type() = default; ~type() = default;
#endif //!sgeGuard_InstanciationRules

#ifndef sgeGuard_StdOutput
#define sgeGuard_StdOutput
#ifdef _DEBUG
#define sge_MESSAGE(msg) std::cout << "[MESSAGE]@" << __FILE__ << "@" << __LINE__ << ": " << msg << std::endl;
#define sge_WARNING(msg) std::cout << "[WARNING]@" << __FILE__ << "@" << __LINE__ << ": " << msg << std::endl;
#define sge_ERROR(msg) std::cerr << "[ERROR]@" << __FILE__ << "@" << __LINE__ << ": " << msg << std::endl; abort();
#define sge_CHECK_GL_ERROR() { if (glGetError() != GL_NO_ERROR){ std::cerr << "[GL_ERROR]@" << __FILE__ << "@" << __LINE__ << std::endl; abort(); } }
#else
#define sge_MESSAGE(msg)
#define sge_WARNING(msg)
#define sge_ERROR(msg)
#endif //!_DEBUG
#endif //!sgeGuard_StdOutput