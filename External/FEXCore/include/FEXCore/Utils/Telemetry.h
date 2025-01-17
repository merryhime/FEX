#pragma once
#include <atomic>
#include <stdint.h>
#include <type_traits>
#include <filesystem>

namespace FEXCore::Telemetry {
#ifndef FEX_DISABLE_TELEMETRY
  class Value;

  class Value final {
    public:
      Value() = default;
      Value(uint64_t Default) : Data {Default} {}

      uint64_t operator*() const { return Data; }
      void operator=(uint64_t Value) { Data = Value; }
      void operator|=(uint64_t Value) { Data |= Value; }
      void operator++(int) { Data++; }

      std::atomic<uint64_t> *GetAddr() { return &Data; }

    private:
      std::atomic<uint64_t> Data;
  };

  enum TelemetryType {
    TYPE_HAS_SPLIT_LOCKS,
    TYPE_16BYTE_SPLIT,
    TYPE_USES_VEX_OPS,
    TYPE_USES_EVEX_OPS,
    TYPE_LAST,
  };

  Value &GetObject(TelemetryType Type);

  void Initialize();
  void Shutdown(std::filesystem::path &ApplicationName);

// Telemetry object declaration
// This returns the internal structure to the telemetry data structures
// One must be careful with placing these in the hot path of code execution
// It can be fairly costly, especially in the static version where it puts barriers in the code
#define FEXCORE_TELEMETRY_STATIC_INIT(Name, Type) static FEXCore::Telemetry::Value &Name = FEXCore::Telemetry::GetObject(FEXCore::Telemetry::Type)
#define FEXCORE_TELEMETRY_INIT(Name, Type) FEXCore::Telemetry::Value &Name = FEXCore::Telemetry::GetObject(FEXCore::Telemetry::Type)
// Telemetry ALU operations
// These are typically 3-4 instructions depending on what you're doing
#define FEXCORE_TELEMETRY_SET(Name, Value) Name = Value
#define FEXCORE_TELEMETRY_OR(Name, Value) Name |= Value
#define FEXCORE_TELEMETRY_INC(Name) Name++

// Returns a pointer to std::atomic<uint64_t>. Can be useful if you are attempting to JIT telemetry accesses for debug purposes
// Not recommended to do telemetry inside JIT code in production code
#define FEXCORE_TELEMETRY_Addr(Name) Name->GetAddr()
#else
  static inline void Initialize() {}
  static inline void Shutdown(std::filesystem::path &) {}

#define FEXCORE_TELEMETRY_STATIC_INIT(Name, Type)
#define FEXCORE_TELEMETRY_INIT(Name, Type)
#define FEXCORE_TELEMETRY(Name, Value) do {} while(0)
#define FEXCORE_TELEMETRY_SET(Name, Value) do {} while(0)
#define FEXCORE_TELEMETRY_OR(Name, Value) do {} while(0)
#define FEXCORE_TELEMETRY_INC(Name) do {} while(0)
#define FEXCORE_TELEMETRY_Addr(Name) reinterpret_cast<std::atomic<uint64_t>*>(nullptr)
#endif
}
