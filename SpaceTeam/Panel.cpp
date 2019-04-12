#include "Panel.hpp"
#include <SpaceTeam/Update.hpp>
#include <HardwareInterface/Types.hpp>

#include <Tcp/Session.hpp>
#include <fmt/format.h>

using st::Panel;

//namespace
//{
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  void OnConnectionError(const std::string& Error)
  {
    std::cerr << "Error: " << Error << std::endl;
  }
//}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Panel::Panel(
  st::UpdateVec& Updates,
  boost::property_tree::ptree& Tree,
  std::shared_ptr<dl::tcp::Session>& pSession)
: mGame(Tree),
  mpSession(pSession),
  mUpdates(Updates),
  mIsConnected(true)
{
  mpSession->GetOnDisconnectSignal().Connect(
    [this]
    {
      std::lock_guard Lock(mMutex);

      mIsConnected = false;
    });

  mpSession->GetOnRxSignal().Connect(
    [this] (const std::string& Bytes)
    {
      if (static_cast<eDeviceID>(Bytes[0]) == eDeviceID::eDigital)
      {
        uint64_t Serial;

        std::memcpy(&Serial, Bytes.data() + 1, 8);

        uint64_t Data;

        std::memcpy(&Data, Bytes.data() + 9, 8);

        std::bitset<8> Bits(Data);

        for (unsigned i = 0; i < 40; ++i)
        {
          mUpdates.Add(st::Update{
            .mPiSerial = Serial,
            .mId = i,
            .mValue = static_cast<uint8_t>(Bits[i])});
        }
      }
    });
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Panel::OnRxDigital(std::string_view Bytes)
{
  // construct an update and add it to the mUpdates;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Panel::OnRxAnalog(std::string_view Bytes)
{
  // construct an update and add it to the mUpdates;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Panel::GetIsConnected() const
{
  std::lock_guard Lock(mMutex);

  return mIsConnected;
}