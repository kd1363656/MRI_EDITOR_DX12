#pragma once

namespace FWK
{
	// より安全なシングルトンを実装するための基底クラス
	template <typename Type>
	class SingletonBase
	{
	public:
	
		static Type& GetInstance()
		{
			static Type l_instance;
			return l_instance;
		}
	
	private:
	
		// コピー演算子削除
		SingletonBase(const SingletonBase<Type>&) = delete;
		// コピー代入演算子削除
		SingletonBase<Type>& operator=(const SingletonBase<Type>&) = delete;
		// 移動コンストラクタ削除
		SingletonBase(SingletonBase<Type>&&) = delete;
		// 移動代入演算子削除
		SingletonBase<Type>& operator=(SingletonBase<Type>&&) = delete;
	
	protected:
	
		SingletonBase         () = default;
		virtual ~SingletonBase() = default;
	};
}