use magnus::{Error, RString, TryConvert, Value};

pub trait NilableConvert<T> {}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum Nilable<T: TryConvert> {
    Nil,
    Value(T),
}

impl TryConvert for Nilable<RString> {
    fn try_convert(val: Value) -> Result<Self, Error> {
        if let Ok(rstring) = RString::try_convert(val) {
            Ok(Self::Value(rstring))
        } else {
            Ok(Self::Nil)
        }
    }
}
