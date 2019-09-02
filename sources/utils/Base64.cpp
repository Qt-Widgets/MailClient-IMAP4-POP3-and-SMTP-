#include "Base64.h"

#include <stdint.h>
#include <stdlib.h>


static char encodingtable[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                              'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                              'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                              'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                              'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                              'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                              'w', 'x', 'y', 'z', '0', '1', '2', '3',
                              '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decodingtable = NULL;
static int modulustable[] = {0, 2, 1};

Base64::Base64()
{
}


void Base64::EncodeBase64(const unsigned char *data,  size_t input_length, unsigned long &output_length, std::string &encodedString)
{  
  output_length = (unsigned long) input_length + 2;
  output_length /= 3;
  output_length *= 4;
  char *encoded_data = new char[output_length+1];//bug
  memset(encoded_data, 0, output_length+1);

  if(encoded_data == NULL)
  {
      output_length = 0;
      encodedString = "";
  }

  for (unsigned int i = 0, j = 0; i < input_length;)
  {

      uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
      uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
      uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

      uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

      encoded_data[j++] = encodingtable[(triple >> 3 * 6) & 0x3F];
      encoded_data[j++] = encodingtable[(triple >> 2 * 6) & 0x3F];
      encoded_data[j++] = encodingtable[(triple >> 1 * 6) & 0x3F];
      encoded_data[j++] = encodingtable[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < modulustable[input_length % 3]; i++)
  {
      encoded_data[output_length - 1 - i] = '=';
  }

  encodedString = encoded_data;

  delete encoded_data;

  return;
}


unsigned char* Base64::DecodeBase64(const char *data,  size_t input_length,  unsigned long &output_length)
{

  if(decodingtable == NULL)
  {
      decodingtable = new char[256];//bug

      for (int i = 0; i < 64; i++)
      {
          decodingtable[(unsigned char) encodingtable[i]] = i;
      }
  }

  if (input_length % 4 != 0) return NULL;

  output_length = (unsigned long) input_length * 3;
  output_length /= 4;
  if (data[input_length - 1] == '=') (output_length)--;
  if (data[input_length - 2] == '=') (output_length)--;

  unsigned char *decoded_data = (unsigned char*)malloc(output_length);
  if (decoded_data == NULL) return NULL;

  for (unsigned int i = 0, j = 0; i < input_length;)
  {

      uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decodingtable[data[i++]];
      uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decodingtable[data[i++]];
      uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decodingtable[data[i++]];
      uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decodingtable[data[i++]];

      uint32_t triple = (sextet_a << 3 * 6)
      + (sextet_b << 2 * 6)
      + (sextet_c << 1 * 6)
      + (sextet_d << 0 * 6);

      if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
      if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
      if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
  }

  return decoded_data;
}

void Base64::DeallocateDecodingTable()
{
  delete decodingtable;
}
