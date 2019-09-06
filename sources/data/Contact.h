#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include "../utils/StringEx.h"

using namespace std;

class Contact
{
public:
    std::string EmailId;

    std::string AlternateEmailIds;
    std::string FirstName;
    std::string MiddleName;
    std::string LastName;
    std::string WebSite;

    std::string AddressLine1;
    std::string AddressLine2;
    std::string City;
    std::string State;
    std::string Country;
    std::string ZipCode;

    std::string WorkPhone;
    std::string HomePhone;
    std::string Fax;
    std::string MobileNo;
    std::string NickName;

    std::string Anniversary;
    std::string Notes;
    std::string Photo;

	Contact()
	{
		EmailId = "-";
		AlternateEmailIds = "-";

		FirstName = "-";
		MiddleName = "-";
		LastName = "-";
		WebSite = "-";

		AddressLine1 = "-";
		AddressLine2 = "-";
		City = "-";
		State = "-";
		Country = "-";
		ZipCode = "-";

		WorkPhone = "-";
		HomePhone = "-";
		Fax = "-";
		MobileNo = "-";
		NickName = "-";

		Anniversary = "-";
		Notes = "-";
		Photo = "-";
	}

	inline void Serialize(std::string& str)
	{
		str.clear();

		str += EmailId + ",";
		str += AlternateEmailIds + ",";
		str += FirstName + ",";
		str += MiddleName + ",";
		str += LastName + ",";
		str += WebSite + ",";
		str += AddressLine1 + ",";
		str += AddressLine2 + ",";
		str += City + ",";
		str += State + ",";
		str += Country + ",";
		str += ZipCode + ",";
		str += WorkPhone + ",";
		str += HomePhone + ",";
		str += Fax + ",";
		str += MobileNo + ",";
		str += NickName + ",";
		str += Anniversary + ",";
		str += Notes + ",";
		str += Photo;
	}

	inline void DeSerialize(std::string& str)
	{
		std::vector<std::string> fields;

		strsplit(str, fields, ',');

		EmailId = fields[0];
		AlternateEmailIds = fields[1];

		FirstName = fields[2];
		MiddleName = fields[3];
		LastName = fields[4];
		WebSite = fields[5];

		AddressLine1 = fields[6];
		AddressLine2 = fields[7];
		City = fields[8];
		State = fields[9];
		Country = fields[10];
		ZipCode = fields[11];

		WorkPhone = fields[12];
		HomePhone = fields[13];
		Fax = fields[14];
		MobileNo = fields[15];
		NickName = fields[16];

		Anniversary = fields[17];
		Notes = fields[18];
		Photo = fields[19];
	}
};

#endif
