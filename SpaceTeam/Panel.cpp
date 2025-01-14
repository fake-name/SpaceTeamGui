#include "Panel.hpp"
#include <HardwareInterface/Types.hpp>
#include <SpaceTeam/Id.hpp>

#include <Tcp/Session.hpp>
#include <fmt/format.h>
#include <bitset>

using st::Panel;

size_t st::Panel::mCount = 0;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Panel::Panel(std::shared_ptr<dl::tcp::Session>& pSession)
: mpSession(pSession),
  mUpdates(),
  mId(mCount++),
  mIsConnected(true),
  moSerial(std::nullopt)
{
  mpSession->GetOnDisconnectSignal().Connect(
    [this]
    {
      mIsConnected = false;
    });

  mpSession->GetOnRxSignal().Connect(
    [this] (const std::string& Bytes)
    {
      if (static_cast<eDeviceID>(Bytes[0]) == eDeviceID::eDigital)
      {
        uint64_t Serial;

        std::memcpy(&Serial, Bytes.data() + 1, 8);

        {
          std::lock_guard Lock(mMutex);

          moSerial = st::SerialId(Serial);
        }

        uint64_t Data;

        std::memcpy(&Data, Bytes.data() + 9, 8);

        std::bitset<64> Bits(Data);

        for (unsigned i = 0; i < 40; ++i)
        {
          mUpdates.Add(st::Update{
            .mPiSerial = st::SerialId(Serial),
            .mId = st::ButtonId(st::ButtonIndex(i), st::SerialId(Serial)),
            .mValue = static_cast<uint8_t>(Bits[i]),
            .mUpdateType = eDeviceID::eDigital});
        }
      }
      else if (static_cast<eDeviceID>(Bytes[0]) == eDeviceID::eAnalog)
      {
        st::SerialId Serial;

        std::memcpy(&Serial, Bytes.data() + 1, 8);

        {
          std::lock_guard Lock(mMutex);

          moSerial = Serial;
        }

        std::array<uint8_t, 24> Data;

        std::memcpy(Data.data(), Bytes.data() + 9, Data.size());

        for (unsigned i = 0; i < Data.size(); ++i)
        {
          mUpdates.Add(st::Update{
            .mPiSerial = Serial,
            .mId = st::ButtonId(st::ButtonIndex(i), Serial),
            .mValue = Data[i],
            .mUpdateType = eDeviceID::eAnalog});
        }
      }
    });
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Panel::GetIsConnected() const
{
  return mIsConnected;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::optional<st::SerialId> Panel::GetSerial() const
{
  std::lock_guard Lock(mMutex);

  return moSerial;
}
