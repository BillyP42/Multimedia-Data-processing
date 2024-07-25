#include<string>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<cstdint>
#include<bit>
#include<iomanip>
#include<concepts>
#include<vector>
//#include<time.h>

using namespace std;

struct Record {
	int32_t RecordDataOffset;
	int8_t RecordAttributes;
	int32_t UniqueId:24;
};

class MOBI {
	char name[32];
	int16_t attributes;
	int16_t version;
	int32_t creation_date;
	int32_t modification_date;
	int32_t backup_date;
	int32_t modificationNumber;
	int32_t appInfoID;
	int32_t sortInfoID;
	char type[5];
	char creator[5];
	int32_t uniqueIdseed;
	int32_t nextRecordList;
	int16_t NumOfRecords;
	ifstream& is_;
	ofstream& os_;
	vector<Record> Records;
public:
	MOBI(std::ifstream& is ,std::ofstream& os):is_(is),os_(os) {}
	int32_t GetRecord(int32_t Id) {
		return Records[Id].RecordDataOffset;
	}
	int16_t GetNum() {
		return NumOfRecords;
	}
	void ReadHeader(ofstream& os) {
		for (auto& x:name) {
			is_.get(x);
		}
		cout<< "Database_name: " << name << endl;
		is_.read(reinterpret_cast<char*>(&attributes), sizeof(attributes));
		is_.read(reinterpret_cast<char*>(&version), sizeof(version));
		is_.read(reinterpret_cast<char*>(&creation_date), sizeof(creation_date));
		creation_date =byteswap(creation_date);
		cout << "Creation_date: " << creation_date << endl;
		is_.seekg(60);
		for (auto& x : type) {
			is_.get(x);
		}
		type[4] = '\0';
		cout << "Type: " << type << endl;
		is_.seekg(64);
		for (auto& x : creator) {
			is_.get(x);
		}
		creator[4] = '\0';
		cout << "Creator: " << creator << endl;
		is_.seekg(76);
		is_.read(reinterpret_cast<char*>(&NumOfRecords), sizeof(NumOfRecords));
		NumOfRecords=byteswap(NumOfRecords);
		
		//is_.read(reinterpret_cast<char*>(&NumOfRecords), sizeof(NumOfRecords));
		cout << "Records: " << NumOfRecords << endl;

		
	}
	void ReadRecords() {
		int32_t tmp;
		int8_t tmp1;
		int32_t tmp2;
		for (size_t i = 0; i < NumOfRecords; i++) {
			is_.read(reinterpret_cast<char*>(&tmp),sizeof(tmp));
			is_.read(reinterpret_cast<char*>(&tmp2), 4);
			tmp = byteswap(tmp);
			tmp2 = byteswap(tmp2);
			cout << i << " - offset: " << tmp << " - id: " << tmp2 << endl;
			Record ciao;
			ciao.RecordDataOffset = tmp;
			ciao.UniqueId = tmp2;
			Records.push_back(ciao);
		}
		
	}
	void ReadFirstRecord() {
		int32_t offset = Records[0].RecordDataOffset;
		int32_t tmp;
		int16_t tmp1;
		is_.seekg(offset);
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1));
		tmp1 = byteswap(tmp1);
		cout << "Compression: " << tmp1 << endl;
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1)); //unused
		is_.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)); //textLenght
		tmp = byteswap(tmp);
		cout << "TextLenght: " << tmp << endl;
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1));
		tmp1 = byteswap(tmp1);
		cout << "RecordLenght: " << tmp1 << endl;
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1));
		tmp1 = byteswap(tmp1);
		cout << "RecordSize: " << tmp1 << endl;
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1));
		tmp1 = byteswap(tmp1);
		cout << "Encryption Type: " << tmp1 << endl;
		is_.read(reinterpret_cast<char*>(&tmp1), sizeof(tmp1));//UNused
		


	}
	void ReadRecordData(int32_t destination_offset) {
		int num = 0;
		int8_t reader;
		is_.seekg(destination_offset);
		do {
			is_.read(reinterpret_cast<char*>(&reader), sizeof(reader));
			if (reader == 0) break;
			if (reader >= 1 && reader <= 8) {
				for (size_t i = 0; i < reader; i++) {
					is_.read(reinterpret_cast<char*>(&reader), sizeof(reader));
					os_.put(reader);
				}
				continue;
			}
			if (reader >= 9 && reader <= 127) {
				os_.put(reader);
				continue;
			}
			if (reader >=-128 && reader <= -65) {
				int8_t tmp_reader;
				int16_t result=0;
				is_.read(reinterpret_cast<char*>(&tmp_reader), sizeof(tmp_reader));
				result = (result << 1) | (1 & 1);
				result = (result << 1) | (0 & 1);
				reader = reader << 2;
				result = (result << 6) | (reader & 1);
				result = (result << 8) | (tmp_reader & 1);
				result -= 3;
				os_.put(result);
				continue;
			}
			if (reader >= -64 && reader <= -1) {
				os_.put(' ');
				reader = (reader << 1);
				os_.put(reader);
				continue;
			}
			

		} while (num++ < 4096);
	}
};

void error(string message) {
	cout << message << endl;
	exit(EXIT_FAILURE);
}
void syntax() {
	printf("SINTAX: MOBIdecoder <file_input> <file_output>");
	exit(EXIT_FAILURE);
}
void ModifyOutput(ofstream& os) {
	int8_t tmp = 0xEF; os.put(tmp);
	tmp = 0xBB; os.put(tmp);
	tmp = 0xBF; os.put(tmp);

}
void Start(const string& input, const string& output) {
	ifstream is(input, ios::binary);
	if (!is)error("errore apertura file output");
	ofstream os(output, ios::binary);
	if (!os)error("errore apertura file output");
	ModifyOutput(os);
	MOBI encoder(is,os);
	encoder.ReadHeader(os);
	encoder.ReadRecords();
	encoder.ReadFirstRecord();
	encoder.ReadRecordData(encoder.GetRecord(1));
	for (size_t i = 2;i<encoder.GetNum();i++) {
		encoder.ReadRecordData(encoder.GetRecord(i));
	}
	
	

}
int main(int argc, char* argv[]) {
	if (argc < 1) syntax();
	Start(argv[1], argv[2]);
}