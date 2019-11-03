#pragma once

#include <string>
#include <map>
#include <vector>

namespace Cjing3D {

struct MemberMetaInfo
{
	static MemberMetaInfo Parse(const std::string& srcBuffer);
	const std::string GenerateRegisterCode()const;
};

struct MethodMetaInfo
{
	static MethodMetaInfo Parse(const std::string& srcBuffer);
	const std::string GenerateRegisterCode()const;

	std::string mFunctionName;
	std::string mClassName;
	bool mIsStatic = false;
};

struct ConstructorMetainfo
{
	static ConstructorMetainfo Parse(const std::string& srcBuffer);
	const std::string GenerateRegisterCode()const;

	std::string mFunctionName;
	std::string mClassName;
	std::vector<std::string> mAargs;
};

class ClassMetaInfo
{
private:
	std::vector<std::string> mParentClass;
	ConstructorMetainfo mConstructorMeta;
	std::vector<MethodMetaInfo> mMethodMetaInfos;
	std::vector<MemberMetaInfo> mMemberMetaInfos;
	std::string mClassName;

public:
	ClassMetaInfo() = default;

	static ClassMetaInfo Parse(const std::string& srcBuffer);
	const std::string GenerateRegisterCode()const;

	std::vector<std::string> GetParentClass()const { return mParentClass; }
	std::string GetName() const { return mClassName; }

	void SetClassName(const std::string& className) { mClassName = className; }
	void PushParentClassName(const std::string& className) { mParentClass.push_back(className); }
	void SetConstructorMetaInfo(const ConstructorMetainfo& info) { mConstructorMeta = info; }
	void PushMethodMetaInfo(const MethodMetaInfo& info) { mMethodMetaInfos.push_back(info); }
	void PushMemberMetaInfo(const MemberMetaInfo& info) { mMemberMetaInfos.push_back(info); }
};

class LuaBindingsGenerator
{
public:
	LuaBindingsGenerator() = default;

	static std::map<std::string, bool> ignoreFiles;		// ºöÂÔµÄÎÄ¼þ

	void ParseAllHeader(const std::string& path);
	bool GenerateSource(const std::string& path);

private:
	void ParseHeader(const std::string& path, const std::string& name);
	
	ClassMetaInfo* mCurrentClassMetaInfo = nullptr;
	std::map<std::string, ClassMetaInfo> mClassMetaInfoMap;
	std::vector<std::string> mDependentHeaders;
};
}