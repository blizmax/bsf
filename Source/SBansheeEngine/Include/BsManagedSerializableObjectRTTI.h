//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableField.h"
#include "BsManagedDiff.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-SEngine
	 *  @{
	 */

	class BS_SCR_BE_EXPORT ManagedSerializableObjectRTTI : public RTTIType<ManagedSerializableObject, IReflectable, ManagedSerializableObjectRTTI>
	{
	private:
		ManagedSerializableObjectInfoPtr getInfo(ManagedSerializableObject* obj)
		{
			return obj->mObjInfo;
		}

		void setInfo(ManagedSerializableObject* obj, ManagedSerializableObjectInfoPtr val)
		{
			obj->mObjInfo = val;
		}

		ManagedSerializableFieldDataEntryPtr getFieldEntry(ManagedSerializableObject* obj, UINT32 arrayIdx)
		{
			Vector<ManagedSerializableFieldInfoPtr>& sequentialFields =
				any_cast_ref<Vector<ManagedSerializableFieldInfoPtr>>(obj->mRTTIData);

			ManagedSerializableFieldInfoPtr field = sequentialFields[arrayIdx];

			ManagedSerializableFieldKeyPtr fieldKey = ManagedSerializableFieldKey::create(field->mParentTypeId, field->mFieldId);
			ManagedSerializableFieldDataPtr fieldData = obj->getFieldData(field);

			return ManagedSerializableFieldDataEntry::create(fieldKey, fieldData);
		}

		void setFieldsEntry(ManagedSerializableObject* obj, UINT32 arrayIdx, ManagedSerializableFieldDataEntryPtr val)
		{
			obj->mCachedData[*val->mKey] = val->mValue;
		}

		UINT32 getNumFieldEntries(ManagedSerializableObject* obj)
		{
			Vector<ManagedSerializableFieldInfoPtr>& sequentialFields =
				any_cast_ref<Vector<ManagedSerializableFieldInfoPtr>>(obj->mRTTIData);

			return (UINT32)sequentialFields.size();
		}

		void setNumFieldEntries(ManagedSerializableObject* obj, UINT32 numEntries)
		{
			// Do nothing
		}

	public:
		ManagedSerializableObjectRTTI()
		{
			addReflectablePtrField("mObjInfo", 0, &ManagedSerializableObjectRTTI::getInfo, &ManagedSerializableObjectRTTI::setInfo);
			addReflectablePtrArrayField("mFieldEntries", 1, &ManagedSerializableObjectRTTI::getFieldEntry, &ManagedSerializableObjectRTTI::getNumFieldEntries, 
				&ManagedSerializableObjectRTTI::setFieldsEntry, &ManagedSerializableObjectRTTI::setNumFieldEntries);
		}

		void onSerializationStarted(IReflectable* obj) override
		{
			ManagedSerializableObject* castObj = static_cast<ManagedSerializableObject*>(obj);

			Vector<ManagedSerializableFieldInfoPtr> sequentialFields;
			ManagedSerializableObjectInfoPtr curType = castObj->mObjInfo;
			while (curType != nullptr)
			{
				for (auto& field : curType->mFields)
				{
					if (field.second->isSerializable())
						sequentialFields.push_back(field.second);
				}

				curType = curType->mBaseClass;
			}

			castObj->mRTTIData = sequentialFields;
		}

		void onSerializationEnded(IReflectable* obj) override
		{
			ManagedSerializableObject* castObj = static_cast<ManagedSerializableObject*>(obj);
			castObj->mRTTIData = nullptr;
		}

		IDiff& getDiffHandler() const override
		{
			static ManagedDiff managedDiffHandler;
			return managedDiffHandler;
		}

		const String& getRTTIName() override
		{
			static String name = "ScriptSerializableObject";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ScriptSerializableObject;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return ManagedSerializableObject::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}