# **Serialization**
* Contains a **Serializer** and a **Deserializer** used in communicaton. Serializer converts ArrayRef to ByteVector and Deserializer convert ByteVector to ArrayRef. ByteVector is used in communication. Apis are in **stl.h**, **serializer.h** and **deserializer.h**.
  ***
  ***
  ### **./stl.h**
  ***
  #### **serialize(sr, str)**
  Serializer for string.
  ##### **Parameters**
  * sr - Serializer reference
  * str - String const reference
  ***
  #### **deserialize(dr, str)**
  Deserializer for string.
  ##### **Parameters**
  * dr - Deserializer reference
  * str - Output string
  ***
  #### **serialize(sr, arr)**
  Serializer for array.
  ##### **Parameters**
  * sr - Serializer reference
  * arr - Array const reference
  ***
  #### **deserialize(dr, arr)**
  Deserializer for array.
  ##### **Parameters**
  * dr - Deserializer reference
  * arr - Output array
  ***
  #### **serialize(sr, vec)**
  Serializer for vector.
  ##### **Parameters**
  * sr - Serializer reference
  * vec - Vector const reference
  ***
  #### **deserialize(dr, vec)**
  Deserializer for vector.
  ##### **Parameters**
  * dr - Deserializer reference
  * vec - Output vector
  ***
  #### **serialize(sr, _map)**
  Serializer for map.
  ##### **Parameters**
  * sr - Serializer reference
  * _map - Map const reference
  ***
  #### **deserialize(dr, _map)**
  Deserializer for map.
  ##### **Parameters**
  * dr - Deserializer reference
  * _map - Output map
  ***
  #### **serialize(sr, _set)**
  Serializer for set (or unordered set).
  ##### **Parameters**
  * sr - Serializer reference
  * _set - Set const reference
  ***
  #### **deserialize(dr, _set)**
  Deserializer for set (or unordered set).
  ##### **Parameters**
  * dr - Deserializer reference
  * _set - Output set
  ***
  #### **serialize(sr, _map)**
  Serializer for map.
  ##### **Parameters**
  * sr - Serializer reference
  * piir - Map const reference
  ***
  #### **deserialize(dr, _map)**
  Deserializer for map.
  ##### **Parameters**
  * dr - Deserializer reference
  * piir - Output map
  ***