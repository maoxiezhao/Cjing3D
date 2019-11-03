#include "generator.h"
#include "helper\fileSystem.h"
#include "helper\logger.h"

#include <set>
#include <atlbase.h>

namespace Cjing3D {

// TODO use config file
std::map<std::string, bool>	LuaBindingsGenerator::ignoreFiles = {
	{ "generates",     true },
	{ "renderer",      true },
	{ "scripts",       true },
};

namespace {

const std::string generateStringPrefix = R"(
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

)";

std::string WrapAutoBindFunction(const std::string& name, const std::string& stringBuffer)
{
	std::string funcName = name + "_AutoBindFunction";
	std::string result = "void " + funcName + "(lua_State* l)\n{\n";
	result = result + stringBuffer;
	result = result + "	\n}\nLUA_FUNCTION_AUTO_BINDER(" + name + "_auto_binder," + funcName + ")";

	return result;
}

std::string GetNextToken(std::string& buffer)
{
	int pos = 0;
	for (auto& c : buffer)
	{
		if (c < 0 || c >= 255) {
			break;
		}

		if (c != '\r' && c != '\n' && !isspace(c)) {
			break;
		}
		pos++;
	}
	buffer = buffer.substr(pos);

	std::string result = "";
	pos = 0;

	char ch = buffer[0];
	switch (ch)
	{
	case ':':
		pos = 1;
		result = "T_EXTEND";
		break;
	case ',':
		pos = 1;
		result = "T_COMMA";
		break;
	case '(':
		pos = 1;
		result = "T_LPAREN";
		break;
	case ')':
		pos = 1;
		result = "T_RPAREN";
		break;
	case '{':
		pos = 1;
		result = "T_LBRACE";
		break;
	case '}':
		pos = 1;
		result = "T_RBRACE";
		break;
	default:
		for (pos = 0; pos < buffer.length(); pos++)
		{
			char ch = buffer[pos];
			if (ch == ':')
			{
				if (pos < buffer.length() - 1 && buffer[pos + 1] == ':'){
					pos++;
					result += ch;
				}
				else{
					break;
				}
			}
			else if (!isalpha(ch) && !isalnum(ch) && ch != '_' && ch != '$' && ch != '&' && ch != '*') {
				break;
			}

			result += ch;
		}
		break;
	}

	buffer = buffer.substr(pos);

	return result;
}

std::string CompactString(const std::string& buffer)
{
	std::string result = buffer;
	int start = 0;
	for (auto& c : result)
	{
		if (c < 0 || c >= 255) {
			break;
		}

		if (c != '\r' && c != '\n' && !isspace(c)) {
			break;
		}
		start++;
	}
	result = result.substr(start);

	int end = result.length() - 1;
	while (end >= 0)
	{
		auto c = result[end];
		if (c < 0 || c >= 255) {
			break;
		}

		if (c != '\r' && c != '\n' && !isspace(c)) {
			break;
		}
		end--;
	}
	result = result.substr(0, end + 1);

	return result;
}

std::string GetCurrentLine(const std::string& buffer)
{
	std::string result = buffer;
	int pos = result.find("\n");
	if (pos == std::string::npos) {
		return result;
	}
	return result.substr(0, pos);
}

std::string GetSentence(const std::string& buffer)
{
	std::string result = buffer;
	int pos = result.find(";");
	if (pos == std::string::npos) {
		pos = result.find("}");

		if (pos == std::string::npos) {
			return result;
		}
	}
	return result.substr(0, pos);
}

std::vector<ClassMetaInfo> GetTopologicalSort(std::map<std::string, ClassMetaInfo> classMetaInfoMap)
{
	std::vector<ClassMetaInfo> sortInfos;

	auto TryGetAvailableClass = [&classMetaInfoMap]()->ClassMetaInfo*
	{
		for (auto& kvp : classMetaInfoMap)
		{
			bool isAvailabled = true;
			auto& classMetaInfo = kvp.second;

			for (auto parentClass : classMetaInfo.GetParentClass())
			{
				if (classMetaInfoMap.find(parentClass) != classMetaInfoMap.end()) {
					isAvailabled = false;
					break;
				}
			}

			if (isAvailabled) {
				return &classMetaInfo;
			}
		}
		return nullptr;
	};

	while (true)
	{
		auto availableClassMetaInfo = TryGetAvailableClass();
		if (availableClassMetaInfo == nullptr) {
			break;
		}

		sortInfos.push_back(*availableClassMetaInfo);
		classMetaInfoMap.erase(availableClassMetaInfo->GetName());
	}

	return sortInfos;
}
}

MemberMetaInfo MemberMetaInfo::Parse(const std::string & srcBuffer)
{
	return MemberMetaInfo();
}

const std::string MemberMetaInfo::GenerateRegisterCode() const
{
	return std::string();
}

MethodMetaInfo MethodMetaInfo::Parse(const std::string & srcBuffer)
{
	MethodMetaInfo metaInfo;
	std::string currentLine = CompactString(GetSentence(srcBuffer));

	auto token = GetNextToken(currentLine);
	if (token == "static") {
		metaInfo.mIsStatic = true;
		token = GetNextToken(currentLine);
	}
	
	if (token == "const") {
		token = GetNextToken(currentLine);
	}

	auto nameToken = GetNextToken(currentLine);
	metaInfo.mFunctionName = nameToken;

	return metaInfo;
}

//  EX:
//	.AddMethod("Shoot", &Gun::Shoot)
//	.AddFunction("PickUp", &Gun::PickUp)
const std::string MethodMetaInfo::GenerateRegisterCode() const
{
	if (mIsStatic)
	{
		return ".AddFunction(\"" + mFunctionName + "\", &" + mClassName + "::" + mFunctionName + ")\n";
	}
	else
	{
		return ".AddMethod(\"" + mFunctionName + "\", &" + mClassName + "::" + mFunctionName + ")\n";
	}
}

ConstructorMetainfo ConstructorMetainfo::Parse(const std::string & srcBuffer)
{
	ConstructorMetainfo metaInfo;
	std::string currentLine = CompactString(GetSentence(srcBuffer));

	auto nameToken = GetNextToken(currentLine);
	metaInfo.mFunctionName = nameToken;

	//ex: Gun(const std::string& name)

	auto token = GetNextToken(currentLine);
	if (token == "T_LPAREN")
	{
		while (token != "T_RPAREN")
		{
			token = GetNextToken(currentLine);
			if (token == "const") {
				token = GetNextToken(currentLine);
			}

			metaInfo.mAargs.push_back(token);

			token = GetNextToken(currentLine);
			token = GetNextToken(currentLine);

			if (token == "T_COMMA") {
				token = GetNextToken(currentLine);
			}
		}
	}

	return metaInfo;
}

const std::string ConstructorMetainfo::GenerateRegisterCode() const
{
	std::string registerString = ".AddConstructor(_LUA_ARGS_(";
	int index = 0;
	for (auto& arg : mAargs)
	{
		if (index > 0) {
			registerString += ", ";
		}

		registerString += arg;
		index++;
	}

	registerString += "))\n";

	return registerString;
}

// Example:
//
// LuaBinder(l)
//	.BeginClass<Gun>("Gun")
//	.AddConstructor(_LUA_ARGS_(std::string))
//	.AddMethod("Shoot", &Gun::Shoot)
//	.AddFunction("PickUp", &Gun::PickUp)
//	.AddMethod("GetName", &Gun::GetName)
//	.AddMember("mValue", &Gun::mValue)
//	.EndClass();

ClassMetaInfo ClassMetaInfo::Parse(const std::string & srcBuffer)
{
	ClassMetaInfo metaInfo;
	std::string currentLine = CompactString(GetCurrentLine(srcBuffer));
	
	auto token = GetNextToken(currentLine);
	if (token != "class") {
		return metaInfo;
	}

	token = GetNextToken(currentLine);
	metaInfo.SetClassName(token);

	token = GetNextToken(currentLine);
	if (token == "T_EXTEND")
	{
		while (true)
		{
			token = GetNextToken(currentLine);
			if (token == "") {
				break;
			}

			if (token != "T_COMMA" && token != "T_LPAREN" && token != "T_LBRACE") {
				metaInfo.mParentClass.push_back(token);
			}
		}
	}

	return metaInfo;
}

const std::string ClassMetaInfo::GenerateRegisterCode() const
{
	std::string registerCode = "LuaBinder(l)\n";
	registerCode += ".BeginClass<" + mClassName + ">(\"" + mClassName + "\")\n";
	registerCode += mConstructorMeta.GenerateRegisterCode();

	for (auto& methodMetaInfo : mMethodMetaInfos) {
		registerCode += methodMetaInfo.GenerateRegisterCode();
	}

	for (auto& memberMetaInfo : mMemberMetaInfos) {
		registerCode += memberMetaInfo.GenerateRegisterCode();
	}

	registerCode += ".EndClass();\n";
	return registerCode;
}

void LuaBindingsGenerator::ParseAllHeader(const std::string & path)
{
	auto files = FileData::EnumerateFiles(path);
	for (const std::string file : files)
	{
		if (ignoreFiles.find(file) != ignoreFiles.end()) {
			continue;
		}

		if (file.find(".h") != std::string::npos) {
			std::string fullPath = file;
			if (path != "") {
				fullPath = path + "/" + file;
			}
			ParseHeader(fullPath, file);
		}
		else if (FileData::IsDirectory(file))
		{
			ParseAllHeader(path + "/" + file);
		}
	}
}

void LuaBindingsGenerator::ParseHeader(const std::string & path, const std::string& name)
{
	OutputDebugString(("Generating lua binding file:" + path + "\n").c_str());
	Logger::Info(("Generating lua binding file:" + path).c_str());

	bool isHeaderDependent = false;

#ifndef USE_CPLUSPLUS_PARSE
	auto srcBuffer = FileData::ReadFile(path);
	auto currentLine = GetCurrentLine(srcBuffer);
	while (currentLine != "")
	{
		const std::string compactCurrentLine = CompactString(currentLine);
		do {
			srcBuffer = srcBuffer.length() > currentLine.length() ? srcBuffer.substr(currentLine.length() + 1) : "";
			currentLine = GetCurrentLine(srcBuffer);
		} 
		while (currentLine == "\r" || currentLine == "\n");
	
		if (compactCurrentLine == "LUA_BINDER_REGISTER_CLASS")
		{
			isHeaderDependent = true;
			mCurrentClassMetaInfo = nullptr;

			ClassMetaInfo classMetaInfo = ClassMetaInfo::Parse(srcBuffer);
			if (classMetaInfo.GetName() != "")
			{
				mClassMetaInfoMap[classMetaInfo.GetName()] = classMetaInfo;
				mCurrentClassMetaInfo = &mClassMetaInfoMap[classMetaInfo.GetName()];
			}
		}
		else if (compactCurrentLine == "LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR")
		{
			if (mCurrentClassMetaInfo != nullptr) 
			{
				ConstructorMetainfo metaInfo = ConstructorMetainfo::Parse(srcBuffer);
				mCurrentClassMetaInfo->SetConstructorMetaInfo(metaInfo);
			}
		}
		else if (compactCurrentLine == "LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION")
		{
			if (mCurrentClassMetaInfo != nullptr)
			{
				MethodMetaInfo metaInfo = MethodMetaInfo::Parse(srcBuffer);
				metaInfo.mClassName = mCurrentClassMetaInfo->GetName();
				mCurrentClassMetaInfo->PushMethodMetaInfo(metaInfo);
			}
		}
	}
#endif

	if (isHeaderDependent) {
		mDependentHeaders.push_back(path);
	}
}

bool LuaBindingsGenerator::GenerateSource(const std::string & path)
{
	std::string generateName = path;
	auto lastIndex = path.find_last_of('.');
	if (lastIndex != std::string::npos)
	{
		generateName = path.substr(0, lastIndex);
	}

	std::string registerCodes = "";
	auto sortedClassMetaInfos = GetTopologicalSort(mClassMetaInfoMap);
	for (auto& classMetaInfo : sortedClassMetaInfos)
	{
		registerCodes = registerCodes + classMetaInfo.GenerateRegisterCode();
		registerCodes = registerCodes + "\n";
	}
	
	// generate include headers
	std::string dependentHeadersString = "";
	for (auto& dependent : mDependentHeaders)
	{
		dependentHeadersString = dependentHeadersString + "#include \"" + dependent + "\"";
	}

	std::string stringBuffer = dependentHeadersString + generateStringPrefix;
	stringBuffer = stringBuffer + WrapAutoBindFunction(generateName, registerCodes);

	// generate final cpp
	FileData::SaveFile(generateName + ".cpp", stringBuffer);
	return true;
}

}