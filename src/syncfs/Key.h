/* Copyright (C) 2015 Alexander Shishenko <GamePad64@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace librevault {
namespace syncfs {

class Key {
	constexpr size_t private_key_size = 32;
	constexpr size_t public_key_size = 33;
	constexpr size_t encryption_key_size = 32;

	std::string secret_s;

	std::vector<uint8_t> payload;

	CryptoPP::DL_PrivateKey_EC<CryptoPP::ECP> private_key;
	CryptoPP::DL_PublicKey_EC<CryptoPP::ECP> public_key;
	std::vector<uint8_t> encryption_key;
public:
	enum Type : char {
		Owner = 'A',	/// Not used now. Will be useful for 'managed' shares for security-related actions. Now equal to ReadWrite.
		ReadWrite = 'B',	/// Signature key, used to sign modified files.
		ReadOnly = 'C',	/// Encryption key (AES-256), used to encrypt blocks/filepaths and used in filepath HMAC
		Download = 'D'	/// Encryption key (AES-256), used to encrypt communications between Librevault nodes
	};

	class secret_error : public std::domain_error {
		secret_error(const char* what) : std::domain_error(what) {}
	};
	class format_error : public secret_error {
		format_error() : secret_error("Secret format mismatch") {}
	};
	class level_error : public secret_error {
		level_error() : secret_error("Key has insufficient privileges for this action") {}
	};
	class crypto_error : public secret_error {
		crypto_error() : secret_error("Cryptographic error. Probably ECDSA domain mismatch") {}
	};

public:	// Yes, I prefer splitting member variables and method declaration
	Key();
	Key(Type type, std::vector<uint8_t> payload);
	Key(std::string string_secret);

	std::vector<uint8_t> get_payload() const {return payload;}
	operator std::string() const {return secret_s;}

	Type getType() const {return secret_s.front();}
	char getCheckChar() const {return secret_s.back();}

	// Key derivers
	Key derive(Type key_type);

	// Payload getters
	std::vector<uint8_t> get_Private_Key() const;
	std::vector<uint8_t> get_Public_Key() const;
	std::vector<uint8_t> get_Encryption_Key() const;

	virtual ~Key();
};

} /* namespace syncfs */
} /* namespace librevault */