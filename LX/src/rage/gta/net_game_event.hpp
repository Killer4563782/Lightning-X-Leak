#pragma once
#include "memory/pointers.h"

#pragma pack(push, 1)
namespace rage
{
#pragma pack(push, 8)
	class CSyncDataBase
	{
	public:
		virtual ~CSyncDataBase() = default;
		virtual bool SerializeDword(uint32_t* dword, int size) = 0; // 1
		virtual bool SerializeWord(uint16_t* word, int size) = 0; // 2
		virtual bool SerializeByte(uint8_t* byte, int size) = 0; // 3
		virtual bool SerializeInt32(int32_t* i, int size) = 0; // 4
		virtual bool SerializeInt16(int16_t* i, int size) = 0; // 5
		virtual bool SerializeSignedByte(int8_t* byte, int size) = 0; // 6
		virtual bool SerializeBool(bool* flag) = 0; // 7
		virtual bool SerializeInt64(int64_t* i, int size) = 0; // 8
		virtual bool SerializeInt32Alt(int32_t* i, int size) = 0; // 9
		virtual bool SerializeInt16Alt(int16_t* i, int size) = 0; // 10
		virtual bool SerializeSignedByteAlt(int8_t* byte, int size) = 0; // 11
		virtual bool SerializeQword(uint64_t* qword, int size) = 0; // 12
		virtual bool SerializeDwordAlt(uint32_t* dword, int size) = 0; // 13
		virtual bool SerializeWordAlt(uint16_t* word, int size) = 0; // 14
		virtual bool SerializeByteAlt(uint8_t* byte, int size) = 0; // 15
		virtual bool SerializeSignedFloat(float* flt, float divisor, int size) = 0; // 16
		virtual bool SerializeFloat(float* flt, float divisor, int size) = 0; // 17
		virtual bool SerializeNetworkId(uint16_t* net_id) = 0; // 18
		virtual bool SerializeVector3(rage::fvector3* vec3, float divisor, int size) = 0; // 19
		virtual bool SerializeQuaternion(void* unk) = 0; // i have no clue what that is
		virtual bool SerializeVector3SignedZComponent(rage::fvector3* vec3, float divisor, int size) = 0; // 21
		virtual bool SerializeOrientation(rage::fvector4* vec4, float size) = 0; // yes, the size is a float
		virtual bool SerializeArray(void* array, int size) = 0;
		virtual bool SerializeString(char* str, int max_length) = 0;
		virtual bool IsSizeCalculator() = 0;
		virtual bool IsSizeCalculator2() = 0;

		enum class Type
		{
			Reader = 1,
			Writer,
			SizeCalculator,
			Logger
		};

		Type m_type;
		void* m_sync_log;
		datBitBuffer* m_buffer;
	};
#pragma pack(pop)

	class netPlayer;

	class datBitBuffer
	{
	public:
		datBitBuffer(void* data, uint32_t size)
		{
			m_data = data;
			m_bitOffset = 0;
			m_maxBit = size * 8;
			m_bitsRead = 0;
			m_curBit = 0;
			m_highestBitsRead = 0;
			m_flagBits = 0;
		}
		uint32_t GetPosition() const
		{
			return m_bitsRead;
		}
		bool Seek(uint32_t bits)
		{
			if (bits >= 0)
			{
				uint32_t length = (m_flagBits & 1) ? m_maxBit : m_curBit;
				if (bits <= length)
					m_bitsRead = bits;
			}
			return false;
		}
		bool WriteBool(bool boolean)
		{
			return g_pointers->g_write_bitbuf_bool(this, boolean, 1);
		}
		bool ReadBool(bool* boolean)
		{
			return g_pointers->g_read_bitbuf_bool(this, boolean, 1);
		}
		bool ReadRockstarId(int64_t* rockstar_id)
		{
			return this->ReadInt64(rockstar_id, sizeof(rockstar_id) * 8);
		}
		uint64_t ReadBits(size_t numBits)
		{
			auto const totalBits = (m_flagBits & 1) ? m_maxBit : m_curBit;
			if ((m_flagBits & 2) || m_bitsRead + numBits > totalBits)
				return 0;
			auto const bufPos = m_bitsRead + m_bitOffset;
			auto const initialBitOffset = bufPos & 0b111;
			auto const start = &((uint8_t*)m_data)[bufPos / 8];
			auto const next = &start[1];
			auto result = (start[0] << initialBitOffset) & 0xff;
			for (auto i = 0; i < ((numBits - 1) / 8); i++)
			{
				result <<= 8;
				result |= next[i] << initialBitOffset;
			}
			if (initialBitOffset)
				result |= next[0] >> (8 - initialBitOffset);
			m_bitsRead += static_cast<uint32_t>(numBits);
			if (m_bitsRead > m_highestBitsRead)
				m_highestBitsRead = m_bitsRead;
			return result >> ((8 - numBits) % 8);
		}
		int GetDataLength() const
		{
			int leftoverBit = (m_curBit % 8) ? 1 : 0;
			return (m_curBit / 8) + leftoverBit;
		}
		uint32_t GetMaxDataLength() {
			return (m_flagBits & 1) != NULL ? m_maxBit : m_curBit;
		}
		bool ReadString(char* string, int bits)
		{
			return g_pointers->g_read_bitbuf_string(this, string, bits);
		}
		bool WriteByte(uint8_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_dword(this, integer, bits);
		}
		bool ReadByte(uint8_t* integer, int bits)
		{
			uint32_t read;
			if (g_pointers->g_read_bitbuf_dword(this, &read, bits))
			{
				*integer = read;
				return true;
			}
			return false;
		}
		bool WriteWord(uint16_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_dword(this, integer, bits);
		}
		bool ReadWord(uint16_t* integer, int bits)
		{
			uint32_t read;
			if (g_pointers->g_read_bitbuf_dword(this, &read, bits))
			{
				*integer = read;
				return true;
			}
			return false;
		}
		bool WriteDword(uint32_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_dword(this, integer, bits);
		}
		bool ReadDword(uint32_t* integer, int bits)
		{
			return g_pointers->g_read_bitbuf_dword(this, integer, bits);
		}
		bool WriteInt32(int32_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_int32(this, integer, bits);
		}
		bool ReadInt32(int32_t* integer, int bits)
		{
			int32_t v8;
			int32_t v9;
			if (ReadDword((uint32_t*)&v8, 1u) && ReadDword((uint32_t*)&v9, bits - 1))
			{
				*integer = v8 + (v9 ^ -v8);
				return true;
			}
			return false;
		}
		bool WriteQWord(uint64_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_qword(this, integer, bits);
		}
		bool ReadQWord(uint64_t* integer, int bits)
		{
			if (bits <= 32)
			{
				uint32_t v10;
				if (ReadDword(&v10, bits))
				{
					*integer = v10;
					return true;
				}
			}
			else
			{
				uint32_t v10, v11;
				if (ReadDword(&v11, 32u) && ReadDword(&v10, bits - 32u))
				{
					*integer = v11 | ((uint64_t)v10 << 32);
					return true;
				}
			}
			return false;
		}
		bool WriteInt64(int64_t integer, int bits)
		{
			return g_pointers->g_write_bitbuf_int64(this, integer, bits);
		}
		bool WriteRockstarId(int64_t rockstar_id)
		{
			return g_pointers->g_write_bitbuf_int64(this, rockstar_id, sizeof(rockstar_id) * 8);
		}
		bool ReadInt64(int64_t* integer, int bits)
		{
			uint32_t v8;
			uint64_t v9;
			if (ReadDword(&v8, 1u) && ReadQWord(&v9, bits - 1))
			{
				*integer = v8 + (v9 ^ -(int64_t)v8);
				return true;
			}
			return false;
		}
		bool WriteArray(void* array, int size)
		{
			return g_pointers->g_write_bitbuf_array(this, array, size, 0);
		}

		void WriteString(const char* string, int max_len)
		{
			auto len = std::min(max_len, (int)strlen(string) + 1);
			bool extended = len > 127;
			Write<bool>(extended, 1);
			Write<int>(len, extended ? 15 : 7);
			WriteArray((void*)string, 8 * len);
		}

		bool ReadArray(PVOID array, int size)
		{
			return g_pointers->g_read_bitbuf_array(this, array, size, 0);
		}

		template<typename T>
		inline T Read(int length)
		{
			static_assert(sizeof(T) <= 4, "maximum of 32 bit read");

			uint32_t val = 0;
			ReadDword(&val, length);

			return T(val);
		}

		template<typename T>
		inline T ReadSigned(int length)
		{
			static_assert(sizeof(T) <= 4, "maximum of 32 bit read");

			int val = 0;
			ReadInt32(&val, length);

			return T(val);
		}

		template<typename T>
		inline void Write(T data, int length)
		{
			static_assert(sizeof(T) <= 8, "maximum of 64 bit write");

			WriteQWord((uint64_t)data, length);
		}

		template<typename T>
		inline void WriteSigned(int length, T data)
		{
			int sign = data < 0;
			int signEx = (data < 0) ? 0xFFFFFFFF : 0;
			int d = (data ^ signEx);

			Write<int>(1, sign);
			Write<int>(length - 1, d);
		}

		inline float ReadFloat(int length, float divisor)
		{
			auto integer = Read<int>(length);

			int max = (1 << length) - 1;
			return static_cast<float>(integer) / max * divisor;
		}

		inline void WriteFloat(int length, float divisor, float value)
		{
			int max = (1 << length) - 1;
			int integer = static_cast<int>((value / divisor) * static_cast<float>(max));

			Write<int>(length, integer);
		}

		inline float ReadSignedFloat(int length, float divisor)
		{
			auto integer = ReadSigned<int>(length);

			int max = (1 << (length - 1)) - 1;
			return static_cast<float>(integer) / static_cast<float>(max) * divisor;
		}

		inline void WriteSignedFloat(int length, float divisor, float value)
		{
			int max = (1 << (length - 1)) - 1;
			int integer = static_cast<int>(value / divisor * max);

			WriteSigned<int>(length, integer);
		}

	public:
		void* m_data;               //0x0000
		uint32_t m_bitOffset;       //0x0008
		uint32_t m_maxBit;          //0x000C
		uint32_t m_bitsRead;        //0x0010
		uint32_t m_curBit;          //0x0014
		uint32_t m_highestBitsRead; //0x0018
		uint8_t m_flagBits;         //0x001C
	};

	/*
		List of message ids.

		Network messages use mandated IDs. This allows us to keep exes network compatible with each other (eg. final logging connecting
		to the retail exe, Steam and non-Steam compatibility, cross-platform multiplayer, etc.), while still keeping the message names
		out of the shipping exes and avoiding a dependency on AutoId constructor call order.

		Any set of exes that need to be network compatible need to have the same list of enums. We can change the values of the enums
		(eg. sort by name, change them for security purposes, etc.) between patches, as long as all exes that need to be kept network
		compatible have the same set of ids.

		Each network message has a corresponding entry in the netMessageId enum list in messageids.h.
		When adding a new network message, add a corresponding entry to the netMessageId enum below.
		When deleting a network message, you can delete the corresponding id from the enum below, as long as it doesn't affect the
		values of any other message ids (there can be gaps/unused message ids).

		Note: message ids for all platforms and configs should be kept in this list on all platforms even if the messages themselves
		are compiled out of certain platforms/configs.

		Message Ids must be < 256, since they are encoded with at most 2 bytes.
	*/

	enum eNetMessage : uint32_t
	{
		CMSG_TEXT_CHAT_STATUS = 0,							// 
		REASSIGN_NEGOTIATE_MSG = 1,							// A class for representing a network message containing a list of objects belonging to a player that has left the network session
		MSG_TRANSITION_TO_GAME_NOTIFY = 2,					// Sent when a transition host has started transition back to game
		CMSG_VOICE_STATUS = 3,								// Sent when a transition host has started transition back to game
		MSG_RESERVE_SLOTS_ACK = 4,							// 
		RL_MSG_SEARCH_REQUEST = 5,							// Broadcast on a LAN to answer session queries.
		MSG_TRANSITION_TO_ACTIVITY_START = 6,				// Sent when leader starts activity join
		MSG_SESSION_ESTABLISHED = 7,						// 
		CMSG_JOIN_RESPONSE = 8,								// Response to a join request
		INFORM_OBJECT_IDS_MSG = 9,							// a message describing objects IDs in use within a specified players ID range The message also contains a list of packed IDs outside of the target players range that are used to replace an IDs with the player range in use by the local machine.
		MSG_TEXT_MESSAGE = 10,								// 
		MSG_SCRIPT_VERIFY_HOST_ACK = 11,					// 
		MSG_BLACKLIST = 12,									// Sent from the host to all peers when a gamer is blacklisted.
		MSG_REQUEST_KICK_FROM_HOST = 13,					// 
		MSG_SCRIPT_NEW_HOST = 14,							// Sent from a new host informing all other machines that it is the new host of the script. 
		NET_KEY_EXCHANGE_SESSION_OFFER = 15,				// 
		REASSIGN_RESPONSE_MSG = 16,							// A class for representing a network message containing a response to a reassignNegotiateMsg or reassignConfirmMsg message
		NET_ICE_SESSION_PONG = 17,							// 
		SN_MSG_MIGRATE_HOST_RESPONSE = 18,					// Sent from a new host to remote peers to notify them of a successful host migration.
		RL_SESSION_DETAIL_RESPONSE = 19,					// Response to a session config request.
		MSG_TRANSITION_GAMER_INSTRUCTION = 20,				// 
		NET_ICE_SESSION_PING = 21,							// Check, Connectivity Check, STUN Check:  A STUN Binding request transaction for the purposes of verifying connectivity. A check is sent from the local candidate to the remote candidate of a candidate pair.
		NON_PHYSICAL_DATA_MSG = 22,							// 
		MSG_SCRIPT_LEAVE = 23,								// Sent when a script participant is leaving the script session
		PLAYER_DATA_MSG = 24,								// A message containing data about a network player when joining a session
		MSG_TRANSITION_LAUNCH = 25,							// Sent from a transition host to all peers to launch a transition
		MSG_SCRIPT_MIGRATE_HOST_FAIL_ACK = 26,				// 
		MSG_TRANSITION_LAUNCH_NOTIFY = 27,					// Sent from a player to all freeroam players to indicate that we are launching a transition session
		MSG_SCRIPT_BOT_JOIN = 28,							// Sent when a bot player is trying to start a new script
		RL_MSG_QOS_PROBE_REQUEST = 29,						// 
		MSG_PARTY_ENTER_GAME = 30,							// 
		SN_MSG_SET_INVITABLE_CMD = 31,						// Sent from host when the invitable setting changes.
		CMSG_PACKED_EVENT_RELIABLES_MSGS = 32,				// This class represents a network message for sending multiple packed game event ACK/Reply messages over the network
		NET_MESSAGE = 33,									// 
		RL_SESSION_DETAIL_REQUEST = 34,						// Sent to a session host to retrieve session config
		SN_MSG_SESSION_MEMBER_IDS = 35,						// Sent from host to verify current session membership. In response peers must locally remove gamers that are not in the session, and add gamers that are in the session but not present locally. Peers must request from the host a snMsgAddGamerToSessionCmd for gamers they don't have locally.
		CMSG_TEXT_MESSAGE = 36,								// 
		MSG_SPLIT_UPDATE_ACK = 37,							// An ack message for an array handler's split update message
		REASSIGN_CONFIRM_MSG = 38,							// A class for representing a network message containing the objects that the local machine is taking control of, and the objects we think the recipient is taking control of
		NET_LAG_PING_MSG = 39,								// Network message used to synchronize time.
		NET_ICE_SESSION_DEBUG_INFO = 40,					// 
		REQUEST_OBJECT_IDS_MSG = 41,						// This message is sent to remote players in a session to request a list of any object IDs within the player's object ID range
		SN_MSG_JOIN_RESPONSE = 42,							// Response to a request to add a gamer to a session.
		MSG_SCRIPT_BOT_LEAVE = 43,							// Sent when a bot participant is leaving the script session
		RL_MSG_QOS_PROBE_RESPONSE = 44,						// 
		SN_MSG_START_MATCH_CMD = 45,						// Sent from the host to a client to tell the client to start the match.
		CROAMING_REQUEST_BUBBLE_MSG = 46,					// 
		MSG_CXN_REQUEST_REMOTE_TIMEOUT = 47,				//

		// A class for representing a network message containing packed game messages that are
		// sent reliably over the network. Reliable messages are particularly expensive in terms
		// of header data, and can cause other bandwidth bottlenecks as they are guaranteed order.
		// Packing the reliable game messages into a single message limits these network costs.
		// The reliable game messages packed into these network messages are as follows:
		//
		// Create messages     - Object creation messages containing data to create a clone of a locally
		//                       owned object to be created on a remote machine
		// Create ACK messages - ACK messages for object creation messages received from a remote machine
		// Remove messages     - Object removal messages containing data to remove a clone of a locally
		//                       owned object on a remote machine
		// Remove ACK messages - ACK messages for object removal messages received from a remote machine
		PACKED_RELIABLES_MSG = 48,

		MSG_SCRIPT_BOT_HANDSHAKE_ACK = 49,					// Sent in response to a bot handshake, when the original handshake could not be processed
		CROAMING_INITIAL_BUBBLE_MSG = 50,					// 
		MSG_SCRIPT_MIGRATE_HOST = 51,						// Sent from the existing host to another machine informing it that it is the new host of the script. 
		MSG_KICK_PLAYER = 52,								// Sent from the host to the player to be kicked.
		SN_MSG_NOT_MIGRATING = 53,							// Sent from a peer to remote peers to indicate that we should not be considered for host migration
		MSG_TRANSITION_TO_ACTIVITY_FINISH = 54,				// Sent when leader completes activity join
		MSG_TRANSITION_PARAMETER_STRING = 55,				// 
		NET_TIME_SYNC_MSG = 56,								// Network message used to synchronize time.
		MSG_RESERVE_SLOTS = 57,								// 
		MSG_PLAYER_CARD_SYNC = 58,							// Message sent with all of the player card stats.
		PACKED_CLONE_SYNC_ACKS_MSG = 59,					// A class for representing a network message containing packed sync ACK messages. Each object update received within a cloneSyncMsg message will be acknowledged by the receiver. Multiple object acknowledgements (ACK codes)can be sent via an individual instance of this class in order to avoid sending unnecessary message header data
		MSG_TRANSITION_PARAMETERS = 60,						// Sent in transition session to maintain parameter states
		MSG_PARTY_LEAVE_GAME = 61,							// 
		MSG_SCRIPT_VERIFY_HOST = 62,						// Sent from a new potential host verifying that it can host the script. This happens when the previous host bombs out and a remaining machine assumes hosting.
		CROAMING_JOIN_BUBBLE_ACK_MSG = 63,					// 
		MSG_SCRIPT_LEAVE_ACK = 64,							// An ack for a msgScriptLeave msg
		SN_MSG_JOIN_REQUEST = 65,							// Sent to a host to request joining the session. The game session host will reserve slots for the group for a short period of time. If the join request is for a group, the caller will be joined to the session, but the rest of the group will only have slots reserved for them.  They must still request to join. The group array must contain at least one entry; that of the caller.
		MSG_REQUEST_TRANSITION_PARAMETERS = 66,				// Sent in transition session to maintain parameter states
		MSG_SCRIPT_JOIN_ACK = 67,							// Sent in reply to a msgScriptJoin from a non-scripthost
		NET_KEY_EXCHANGE_SESSION_ANSWER = 68,				// 
		NET_ICE_SESSION_PORT_OPENER = 69,					// 
		MSG_RADIO_STATION_SYNC = 70,						// Sent from the host to sync the current session radio settings
		MSG_RADIO_STATION_SYNC_REQUEST = 71,				// Request radio sync from host
		SN_MSG_CONFIG_REQUEST = 72,							// Sent to a session host to retrieve session config
		MSG_CXN_RELAY_ADDR_CHANGED = 73,					// 
		NET_ICE_SESSION_ANSWER_OLD = 74,					// unversioned struct - no longer used
		MSG_SCRIPT_BOT_HANDSHAKE = 75,						// Sent by a player bot to all other participants when the bot is accepted by the script host
		CROAMING_JOIN_BUBBLE_MSG = 76,						// 
		MSG_SCRIPT_JOIN_HOST_ACK = 77,						// Sent in reply to a msgScriptJoin from a script host
		CLONE_SYNC_MSG = 78,								// a message containing one or more object updates (sync messages)
		CMSG_PACKED_EVENTS = 79,							// This class represents a network message for sending multiple packed game event messages over the network
		MSG_TRANSITION_TO_GAME_START = 80,					// Sent when a transition host has started transition back to game
		MSG_CHECK_QUEUED_JOIN_REQUEST = 81,					// Check queued join request (client -> host)
		MSG_SESSION_ESTABLISHED_REQUEST = 82,				// 
		SN_MSG_REMOVE_GAMERS_FROM_SESSION_CMD = 83,			// Sent to notify a peer to remove a gamer from its local instance of a session.
		SN_MSG_REQUEST_GAMER_INFO = 84,						// Sent to host to request that the host send us the add gamer command for an individual gamer. This is sent when a peer receives a snMsgSessionMemberIds message and realizes it doesn't locally have all members of the session.
		NET_COMPLAINT_MSG = 85,								// Network message used to send a peer complaint.
		NET_ICE_SESSION_OFFER_OLD = 86,						// unversioned struct - no longer used
		MSG_SCRIPT_HANDSHAKE = 87,							// Sent by a script client to all other clients when the client is accepted by the script host
		SN_MSG_HOST_LEFT_WHILST_JOINING_CMD = 88,			// Sent from host to guest to notify that the host left during the join process.
		MSG_CHECK_QUEUED_JOIN_REQUEST_REPLY = 89,			// Check queued join request (client -> host)
		SN_MSG_CHANGE_SESSION_ATTRIBUTES_CMD = 90,			// Sent from host when the session attributes change.
		MSG_SCRIPT_HANDSHAKE_ACK = 91,						// An ack for a msgScriptHandshake msg
		MSG_SCRIPT_JOIN = 92,								// Sent when a player is trying to start a new script
		MSG_UPDATE_ACK = 93,								// An ack message for an array handler's update message
		MSG_CHECK_QUEUED_JOIN_REQUEST_INVITE_REPLY = 94,	// Check queued join request (client -> host)
		SN_MSG_CONFIG_RESPONSE = 95,						// Response to a session config request.
		MSG_UPDATE = 96,									// Message potentially containing a number of array handler updates, each containing updates for a number of array elements.
		CMSG_PLAYER_IS_TYPING = 97,							// 
		SN_MSG_SESSION_ACCEPT_CHAT = 98,					// Sent to accept the chat request. We join the session as soon as we receive the event. This signals that chat can start (for the host)
		MSG_SCRIPT_BOT_JOIN_ACK = 99,						// Sent in reply to a msgScriptBotJoin from a script host
		SN_MSG_ADD_GAMER_TO_SESSION_CMD = 100,				// Sent from host to guest to notify it add a gamer to its local instance of a session.
		ACTIVATE_NETWORK_BOT_MSG = 101,						// This message is sent by a network bot when it is activated on the machine it was created by. This ensures network bots are not processed by remote machines until they have finished joining the session on the machine that created them
		SN_MSG_MIGRATE_HOST_REQUEST = 102,					// Sent from a new host to remote peers to notify them of a successful host migration.
		MSG_SCRIPT_HOST_REQUEST = 103,						// Sent from a script client to the current host requesting to become the new host. 
		CMSG_JOIN_REQUEST = 104,							// Message sent to a host to request to join a network session
		SN_MSG_BASE = 105,									// Base class for session messages. Contains the session id in order to distinguish messages belonging to different sessions.
		MSG_PLAYER_CARD_REQUEST = 106,						// Message sent to request a player card stats.
		RL_MSG_SEARCH_RESPONSE = 107,						// Broadcast on a LAN to search for LAN sessions.
		MSG_DEBUG_TIME_SCALE = 108,							// Sent when a player is trying to start a new script
		MSG_DEBUG_TIME_PAUSE = 109,							// 
		MSG_DEBUG_NO_TIMEOUTS = 110,						// 
		MSG_DEBUG_ADD_FAIL_MARK = 111,						// 
		MSG_SYNC_DR_DISPLAY = 112,							// 
		SN_MSG_SESSION_INFO = 113,							// Sent to give a remote connection information about a session
		SN_MSG_SESSION_INFO_RESPONSE = 114,					// Sent to acknowledge session info receipt
		SN_MSG_SESSION_JOIN_REQUEST = 115,					// Sent to indicate answer to request
		SN_MSG_SESSION_JOIN_REQUEST_RESPONSE = 116,			// Sent to acknowledge join response receipt
		CMSG_VOICE_CHAT_REQUEST = 117,						// Debug bank to start a chat with a certain user.
		CMSG_VOICE_CHAT_REPLY = 118,						// Debug bank to start a chat with a certain user.
		CMSG_VOICE_CHAT_END = 119,							// Debug bank to start a chat with a certain user.
		VOICE_CHAT_USER_MSG = 120,							// Hang on to ChatIntegrationLayer shared pointer to make sure that the ref count doesn't decrement to zero before VoiceChatDurango is done with it.
		SESSION_INFO_MSG = 121,								// 
		REGISTER_COMPLETE_MSG = 122,						// 
		START_COMMAND_MSG = 123,							// 
		QA_NR_FOO_MSG = 124,								// 
		QA_NR_BAR_MSG = 125,								// 
		MSG_DEBUG_STALL = 126,								// Message sent to connected players when there is a stall.
		NET_ICE_SESSION_OFFER = 127,						// 
		NET_ICE_SESSION_ANSWER = 128,						// 
		MSG_LOST_HOST_CONNECTION = 129,						// Sent from a peer to host
		CROAMING_BUBBLE_REQUIRED_CHECK_MSG = 130,			// 
		CROAMING_BUBBLE_REQUIRED_RESPONSE_MSG = 131,		// 
		NET_ROUTE_REQUEST_MSG = 132,						// 
		NET_ROUTE_REPLY_MSG = 133,							// 
		NET_ROUTE_ERR_MSG = 134,							// 
		NET_ROUTE_CHANGE_REQUEST_MSG = 135,					// 
		NET_ROUTE_CHANGE_REPLY_MSG = 136,					// 
		NET_ICE_SESSION_RELAY_ROUTE_CHECK = 137,			//
		MSG_INVALID = 0xFFFFF,
		MsgBattlEyeCmd = 0x8F,
	};

	enum class eEventNetworkType : int64_t
	{
		// AI Events
		EVENT_ACQUAINTANCE_PED_DISLIKE,
		EVENT_ACQUAINTANCE_PED_HATE,
		EVENT_ACQUAINTANCE_PED_LIKE,
		EVENT_ACQUAINTANCE_PED_RESPECT,
		EVENT_ACQUAINTANCE_PED_WANTED,
		EVENT_ACQUAINTANCE_PED_DEAD,
		EVENT_AGITATED,
		EVENT_AGITATED_ACTION,
		EVENT_ENCROACHING_PED,
		EVENT_CALL_FOR_COVER,
		EVENT_CAR_UNDRIVEABLE,
		EVENT_CLIMB_LADDER_ON_ROUTE,
		EVENT_CLIMB_NAVMESH_ON_ROUTE,
		EVENT_COMBAT_TAUNT,
		EVENT_COMMUNICATE_EVENT,
		EVENT_COP_CAR_BEING_STOLEN,
		EVENT_CRIME_REPORTED,
		EVENT_DAMAGE,
		EVENT_DEAD_PED_FOUND,
		EVENT_DEATH,
		EVENT_DRAGGED_OUT_CAR,
		EVENT_DUMMY_CONVERSION,
		EVENT_EXPLOSION,
		EVENT_EXPLOSION_HEARD,
		EVENT_FIRE_NEARBY,
		EVENT_FLUSH_TASKS,
		EVENT_FOOT_STEP_HEARD,
		EVENT_GET_OUT_OF_WATER,
		EVENT_GIVE_PED_TASK,
		EVENT_GUN_AIMED_AT,
		EVENT_HELP_AMBIENT_FRIEND,
		EVENT_INJURED_CRY_FOR_HELP,
		EVENT_CRIME_CRY_FOR_HELP,
		EVENT_IN_AIR,
		EVENT_IN_WATER,
		EVENT_INCAPACITATED,
		EVENT_LEADER_ENTERED_CAR_AS_DRIVER,
		EVENT_LEADER_ENTERED_COVER,
		EVENT_LEADER_EXITED_CAR_AS_DRIVER,
		EVENT_LEADER_HOLSTERED_WEAPON,
		EVENT_LEADER_LEFT_COVER,
		EVENT_LEADER_UNHOLSTERED_WEAPON,
		EVENT_MELEE_ACTION,
		EVENT_MUST_LEAVE_BOAT,
		EVENT_NEW_TASK,
		EVENT_NONE,
		EVENT_OBJECT_COLLISION,
		EVENT_ON_FIRE,
		EVENT_OPEN_DOOR,
		EVENT_SHOVE_PED,
		EVENT_PED_COLLISION_WITH_PED,
		EVENT_PED_COLLISION_WITH_PLAYER,
		EVENT_PED_ENTERED_MY_VEHICLE,
		EVENT_PED_JACKING_MY_VEHICLE,
		EVENT_PED_ON_CAR_ROOF,
		EVENT_PED_TO_CHASE,
		EVENT_PED_TO_FLEE,
		EVENT_PLAYER_COLLISION_WITH_PED,
		EVENT_PLAYER_LOCK_ON_TARGET,
		EVENT_POTENTIAL_BE_WALKED_INTO,
		EVENT_POTENTIAL_BLAST,
		EVENT_POTENTIAL_GET_RUN_OVER,
		EVENT_POTENTIAL_WALK_INTO_FIRE,
		EVENT_POTENTIAL_WALK_INTO_OBJECT,
		EVENT_POTENTIAL_WALK_INTO_VEHICLE,
		EVENT_PROVIDING_COVER,
		EVENT_RADIO_TARGET_POSITION,
		EVENT_RAN_OVER_PED,
		EVENT_REACTION_COMBAT_VICTORY,
		EVENT_REACTION_ENEMY_PED,
		EVENT_REACTION_INVESTIGATE_DEAD_PED,
		EVENT_REACTION_INVESTIGATE_THREAT,
		EVENT_REQUEST_HELP_WITH_CONFRONTATION,
		EVENT_RESPONDED_TO_THREAT,
		EVENT_REVIVED,
		EVENT_SCRIPT_COMMAND,
		EVENT_SHOCKING_BROKEN_GLASS,
		EVENT_SHOCKING_CAR_ALARM,
		EVENT_SHOCKING_CAR_CHASE,
		EVENT_SHOCKING_CAR_CRASH,
		EVENT_SHOCKING_BICYCLE_CRASH,
		EVENT_SHOCKING_CAR_PILE_UP,
		EVENT_SHOCKING_CAR_ON_CAR,
		EVENT_SHOCKING_DANGEROUS_ANIMAL,
		EVENT_SHOCKING_DEAD_BODY,
		EVENT_SHOCKING_DRIVING_ON_PAVEMENT,
		EVENT_SHOCKING_BICYCLE_ON_PAVEMENT,
		EVENT_SHOCKING_ENGINE_REVVED,
		EVENT_SHOCKING_EXPLOSION,
		EVENT_SHOCKING_FIRE,
		EVENT_SHOCKING_GUN_FIGHT,
		EVENT_SHOCKING_GUNSHOT_FIRED,
		EVENT_SHOCKING_HELICOPTER_OVERHEAD,
		EVENT_SHOCKING_PARACHUTER_OVERHEAD,
		EVENT_SHOCKING_PED_KNOCKED_INTO_BY_PLAYER,
		EVENT_SHOCKING_HORN_SOUNDED,
		EVENT_SHOCKING_IN_DANGEROUS_VEHICLE,
		EVENT_SHOCKING_INJURED_PED,
		EVENT_SHOCKING_MAD_DRIVER,
		EVENT_SHOCKING_MAD_DRIVER_EXTREME,
		EVENT_SHOCKING_MAD_DRIVER_BICYCLE,
		EVENT_SHOCKING_MUGGING,
		EVENT_SHOCKING_NON_VIOLENT_WEAPON_AIMED_AT,
		EVENT_SHOCKING_PED_RUN_OVER,
		EVENT_SHOCKING_PED_SHOT,
		EVENT_SHOCKING_PLANE_FLY_BY,
		EVENT_SHOCKING_POTENTIAL_BLAST,
		EVENT_SHOCKING_PROPERTY_DAMAGE,
		EVENT_SHOCKING_RUNNING_PED,
		EVENT_SHOCKING_RUNNING_STAMPEDE,
		EVENT_SHOCKING_SEEN_CAR_STOLEN,
		EVENT_SHOCKING_SEEN_CONFRONTATION,
		EVENT_SHOCKING_SEEN_GANG_FIGHT,
		EVENT_SHOCKING_SEEN_INSULT,
		EVENT_SHOCKING_SEEN_MELEE_ACTION,
		EVENT_SHOCKING_SEEN_NICE_CAR,
		EVENT_SHOCKING_SEEN_PED_KILLED,
		EVENT_SHOCKING_SEEN_VEHICLE_TOWED,
		EVENT_SHOCKING_SEEN_WEAPON_THREAT,
		EVENT_SHOCKING_SEEN_WEIRD_PED,
		EVENT_SHOCKING_SEEN_WEIRD_PED_APPROACHING,
		EVENT_SHOCKING_SIREN,
		EVENT_SHOCKING_STUDIO_BOMB,
		EVENT_SHOCKING_VISIBLE_WEAPON,
		EVENT_SHOT_FIRED,
		EVENT_SHOT_FIRED_BULLET_IMPACT,
		EVENT_SHOT_FIRED_WHIZZED_BY,
		EVENT_FRIENDLY_AIMED_AT,
		EVENT_FRIENDLY_FIRE_NEAR_MISS,
		EVENT_SHOUT_BLOCKING_LOS,
		EVENT_SHOUT_TARGET_POSITION,
		EVENT_STATIC_COUNT_REACHED_MAX,
		EVENT_STUCK_IN_AIR,
		EVENT_SUSPICIOUS_ACTIVITY,
		EVENT_SWITCH_2_NM_TASK,
		EVENT_UNIDENTIFIED_PED,
		EVENT_VEHICLE_COLLISION,
		EVENT_VEHICLE_DAMAGE_WEAPON,
		EVENT_VEHICLE_ON_FIRE,
		EVENT_WHISTLING_HEARD,
		EVENT_DISTURBANCE,
		EVENT_ENTITY_DAMAGED,
		EVENT_ENTITY_DESTROYED,
		EVENT_WRITHE,
		EVENT_HURT_TRANSITION,
		EVENT_PLAYER_UNABLE_TO_ENTER_VEHICLE,
		EVENT_SCENARIO_FORCE_ACTION,
		EVENT_STAT_VALUE_CHANGED,
		EVENT_PLAYER_DEATH,
		EVENT_PED_SEEN_DEAD_PED,
		NUM_AI_EVENTTYPE,

		// Network events
		EVENT_NETWORK_PLAYER_JOIN_SESSION,
		EVENT_NETWORK_PLAYER_LEFT_SESSION,
		EVENT_NETWORK_PLAYER_JOIN_SCRIPT,
		EVENT_NETWORK_PLAYER_LEFT_SCRIPT,
		EVENT_NETWORK_STORE_PLAYER_LEFT,
		EVENT_NETWORK_SESSION_START,
		EVENT_NETWORK_SESSION_END,
		EVENT_NETWORK_START_MATCH,
		EVENT_NETWORK_END_MATCH,
		EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_STALL,
		EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_COMPLAINTS,
		EVENT_NETWORK_CONNECTION_TIMEOUT,
		EVENT_NETWORK_PED_DROPPED_WEAPON,
		EVENT_NETWORK_PLAYER_SPAWN,
		EVENT_NETWORK_PLAYER_COLLECTED_PICKUP,
		EVENT_NETWORK_PLAYER_COLLECTED_AMBIENT_PICKUP,
		EVENT_NETWORK_PLAYER_COLLECTED_PORTABLE_PICKUP,
		EVENT_NETWORK_PLAYER_DROPPED_PORTABLE_PICKUP,
		EVENT_NETWORK_INVITE_ARRIVED,
		EVENT_NETWORK_INVITE_ACCEPTED,
		EVENT_NETWORK_INVITE_CONFIRMED,
		EVENT_NETWORK_INVITE_REJECTED,
		EVENT_NETWORK_SUMMON,
		EVENT_NETWORK_SCRIPT_EVENT,
		EVENT_NETWORK_PLAYER_SIGNED_OFFLINE,
		EVENT_NETWORK_SIGN_IN_STATE_CHANGED,
		EVENT_NETWORK_SIGN_IN_CHANGE_ACTIONED,
		EVENT_NETWORK_NETWORK_ROS_CHANGED,
		EVENT_NETWORK_NETWORK_BAIL,
		EVENT_NETWORK_HOST_MIGRATION,
		EVENT_NETWORK_FIND_SESSION,
		EVENT_NETWORK_HOST_SESSION,
		EVENT_NETWORK_JOIN_SESSION,
		EVENT_NETWORK_JOIN_SESSION_RESPONSE,
		EVENT_NETWORK_CHEAT_TRIGGERED,
		EVENT_NETWORK_DAMAGE_ENTITY,
		EVENT_NETWORK_PLAYER_ARREST,
		EVENT_NETWORK_TIMED_EXPLOSION,
		EVENT_NETWORK_PRIMARY_CLAN_CHANGED,
		EVENT_NETWORK_CLAN_JOINED,
		EVENT_NETWORK_CLAN_LEFT,
		EVENT_NETWORK_CLAN_INVITE_RECEIVED,
		EVENT_VOICE_SESSION_STARTED,
		EVENT_VOICE_SESSION_ENDED,
		EVENT_VOICE_CONNECTION_REQUESTED,
		EVENT_VOICE_CONNECTION_RESPONSE,
		EVENT_VOICE_CONNECTION_TERMINATED,
		EVENT_TEXT_MESSAGE_RECEIVED,
		EVENT_CLOUD_FILE_RESPONSE,
		EVENT_NETWORK_PICKUP_RESPAWNED,
		EVENT_NETWORK_PRESENCE_STAT_UPDATE,
		EVENT_NETWORK_PED_LEFT_BEHIND,
		EVENT_NETWORK_INBOX_MSGS_RCVD,
		EVENT_NETWORK_ATTEMPT_HOST_MIGRATION,
		EVENT_NETWORK_INCREMENT_STAT,
		EVENT_NETWORK_SESSION_EVENT,
		EVENT_NETWORK_TRANSITION_STARTED,
		EVENT_NETWORK_TRANSITION_EVENT,
		EVENT_NETWORK_TRANSITION_MEMBER_JOINED,
		EVENT_NETWORK_TRANSITION_MEMBER_LEFT,
		EVENT_NETWORK_TRANSITION_PARAMETER_CHANGED,
		EVENT_NETWORK_CLAN_KICKED,
		EVENT_NETWORK_TRANSITION_STRING_CHANGED,
		EVENT_NETWORK_TRANSITION_GAMER_INSTRUCTION,
		EVENT_NETWORK_PRESENCE_INVITE,
		EVENT_NETWORK_PRESENCE_INVITE_REMOVED,
		EVENT_NETWORK_PRESENCE_INVITE_REPLY,
		EVENT_NETWORK_CASH_TRANSACTION_LOG,
		EVENT_NETWORK_CLAN_RANK_CHANGE,
		EVENT_NETWORK_VEHICLE_UNDRIVABLE,
		EVENT_NETWORK_PRESENCE_TRIGGER,
		EVENT_NETWORK_EMAIL_RECEIVED,
		EVENT_NETWORK_FOLLOW_INVITE_RECEIVED,
		EVENT_NETWORK_ADMIN_INVITED,
		EVENT_NETWORK_SPECTATE_LOCAL,
		EVENT_NETWORK_CLOUD_EVENT,
		EVENT_NETWORK_SHOP_TRANSACTION,
		EVENT_NETWORK_PERMISSION_CHECK_RESULT,
		EVENT_NETWORK_APP_LAUNCHED,
		EVENT_NETWORK_ONLINE_PERMISSIONS_UPDATED,
		EVENT_NETWORK_SYSTEM_SERVICE_EVENT,
		EVENT_NETWORK_REQUEST_DELAY,
		EVENT_NETWORK_SOCIAL_CLUB_ACCOUNT_LINKED,
		EVENT_NETWORK_SCADMIN_PLAYER_UPDATED,
		EVENT_NETWORK_SCADMIN_RECEIVED_CASH,
		EVENT_NETWORK_CLAN_INVITE_REQUEST_RECEIVED,
		EVENT_NETWORK_MARKETING_EMAIL_RECEIVED,
		EVENT_NETWORK_STUNT_PERFORMED,
		EVENT_NETWORK_FIRED_DUMMY_PROJECTILE,
		EVENT_NETWORK_PLAYER_ENTERED_VEHICLE,
		EVENT_NETWORK_PLAYER_ACTIVATED_SPECIAL_ABILITY,
		EVENT_NETWORK_PLAYER_DEACTIVATED_SPECIAL_ABILITY,
		EVENT_NETWORK_PLAYER_SPECIAL_ABILITY_FAILED_ACTIVATION,
		EVENT_NETWORK_FIRED_VEHICLE_PROJECTILE,
		EVENT_NETWORK_SC_MEMBERSHIP_STATUS,
		EVENT_NETWORK_SC_BENEFITS_STATUS,
		NUM_NETWORK_EVENTTYPE,

		EVENT_ERRORS_UNKNOWN_ERROR,
		EVENT_ERRORS_ARRAY_OVERFLOW,
		EVENT_ERRORS_INSTRUCTION_LIMIT,
		EVENT_ERRORS_STACK_OVERFLOW,
		NUM_ERRORS_EVENTTYPE,

		EVENT_INVALID = -1,

	};

	class CEventNetwork
	{
	public:
		DEFINE_RAGE_RTTI(rage::CEventNetwork);
		virtual void unk_0008() = 0; //0x0008 (1)
		virtual void unk_0010() = 0; //0x0010 (2)
		virtual eEventNetworkType get_type() = 0; //0x0018 (3)
		virtual void unk_0020() = 0; //0x0020 (4)
		virtual void unk_0028() = 0; //0x0028 (5)
		virtual bool get_extra_information(void* info_array, int check) = 0; //0x0030 (6)
		virtual void unk_0038() = 0; //0x0038 (7)
	};                                                                       //Size: 0x0008

	class sPickupCollectedData
	{
	public:
		alignas(8) int m_placement_reference;
		alignas(8) Player m_player_index;
		alignas(8) uint32_t m_pickup_hash;
		alignas(8) int m_pickup_amount;
		alignas(8) uint32_t m_pickup_model;
		alignas(8) int m_pickup_collected;
	};

	class sAmbientPickupCollectedData
	{
	public:
		alignas(8) uint32_t m_pickup_hash;
		alignas(8) int m_pickup_amount;
		alignas(8) Player m_player_index;
		alignas(8) uint32_t m_pickup_model;
		alignas(8) bool m_player_gift;
		alignas(8) bool m_dropped_by_ped;
		alignas(8) int m_pickup_collected;
		alignas(8) int m_pickup_index;
	};

	class sPortablePickupCollectedData
	{
	public:
		alignas(8) int m_pickup_id;
		alignas(8) uint16_t m_pickup_net_id;
		alignas(8) Player m_player_index;
		alignas(8) uint32_t m_pickup_model;
	};

	class sEntityDamagedData
	{
	public:
		alignas(8) Entity m_victim_index;               //0x0000
		alignas(8) Entity m_damager_index;              //0x0008
		alignas(8) float m_damage;                      //0x0010
		alignas(8) float m_endurance_damage;            //0x0018
		alignas(8) bool m_victim_incapacitated;         //0x0020
		alignas(8) bool m_victim_destroyed;             //0x0028
		alignas(8) int m_weapon_used;                   //0x0030
		alignas(8) float m_victim_speed;                //0x0038
		alignas(8) float m_damager_speed;               //0x0040
		alignas(8) bool m_is_responsible_for_collision; //0x0048
		alignas(8) bool m_is_headshot;                  //0x0050
		alignas(8) bool m_is_with_melee_weapon;         //0x0058
		alignas(8) int m_hit_material;                  //0x0060
	};                                                  //Size: 0x0068
	static_assert(sizeof(sEntityDamagedData) == 0x68);

	class netGameEvent
	{
	public:
		virtual ~netGameEvent() = default;

		virtual const char* get_name()
		{
			return 0;
		};
		virtual bool is_in_scope(netPlayer* player)
		{
			return 0;
		};
		virtual bool time_to_resend(uint32_t time)
		{
			return 0;
		};
		virtual bool can_change_scope()
		{
			return 0;
		};

		virtual void prepare_data(datBitBuffer* buffer, netPlayer* source_player, netPlayer* target_player) {};
		virtual void handle_data(datBitBuffer* buffer, netPlayer* source_player, netPlayer* target_player) {};

		virtual bool decide(netPlayer* source_player, netPlayer* target_player)
		{
			return 0;
		};

		virtual void prepare_reply(datBitBuffer* buffer, netPlayer* reply_player) {};
		virtual void handle_reply(datBitBuffer* buffer, netPlayer* souce_player) {};

		virtual void prepare_extra_data(datBitBuffer* buffer, bool is_reply, netPlayer* player, netPlayer* player2) {};
		virtual void handle_extra_data(datBitBuffer* buffer, bool is_reply, netPlayer* player, netPlayer* player2) {};

	private:
		virtual void unk_0x60() {};
		virtual void unk_0x68() {};
		virtual void unk_0x70() {};
		virtual void unk_0x78() {};

	public:
		virtual bool operator==(netGameEvent const& other)
		{
			return 0;
		};
		virtual bool operator!=(netGameEvent const& other)
		{
			return 0;
		};

		virtual bool must_persist()
		{
			return 0;
		};
		virtual bool must_persist_when_out_of_scope()
		{
			return 0;
		};
		virtual bool has_timed_out()
		{
			return 0;
		};

	public:
		uint16_t m_id;         // 0x08
		bool m_requires_reply; // 0x0A
	private:
		char m_padding1[0x05]; // 0x0B
	public:
		netPlayer* m_source_player; // 0x10
		netPlayer* m_target_player; // 0x18
		uint32_t m_resend_time;     // 0x20
	private:
		uint16_t m_0x24; // 0x24
		uint8_t m_0x26;  // 0x26
		uint8_t m_0x27;  // 0x27
		uint32_t m_0x28; // 0x28
		char m_padding2[0x04];
	};
	static_assert(sizeof(rage::netGameEvent) == 0x30);
}

class CScriptedGameEvent : public rage::netGameEvent
{
public:
	char m_padding[0x40];    // 0x30
	int64_t m_args[54];      // 0x70
	uint32_t m_bitset;       // 0x220
	uint32_t m_args_size;    // 0x224
};

class CNetworkIncrementStatEvent : public rage::netGameEvent
{
public:
	Hash m_stat;       // 0x30
	uint32_t m_amount; // 0x34
};

class CDoorBreakEvent : public rage::netGameEvent
{
public:
	uint16_t m_vehicle_id; // 0x30
	uint8_t m_door_id;     // 0x32
};
#pragma pack(pop)