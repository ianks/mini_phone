use std::str::FromStr;

use magnus::{exception::arg_error, function, Object, RString, Value};
use phonenumber::PhoneNumber;

use crate::{phone_number::RubyPhoneNumber, root_module, traits::Nilable};

pub struct ClassInterface;

impl ClassInterface {
    pub fn parse(input: Value) -> Result<RubyPhoneNumber, magnus::Error> {
        RubyPhoneNumber::new(input)
    }

    pub fn is_valid(rstring: Nilable<RString>) -> Result<bool, magnus::Error> {
        let Nilable::Value(rstring) = rstring else {
            return Ok(false);
        };
        let phone_number = unsafe { rstring.as_str() }?;
        if let Ok(phone_number) = PhoneNumber::from_str(phone_number) {
            Ok(phonenumber::is_valid(&phone_number))
        } else {
            Ok(false)
        }
    }

    pub fn is_possible(rstring: Nilable<RString>) -> Result<bool, magnus::Error> {
        let Nilable::Value(rstring) = rstring else {
            return Ok(false);
        };
        if let Ok(phone_number) = unsafe { rstring.as_str() } {
            Ok(phonenumber::is_viable(phone_number))
        } else {
            Ok(false)
        }
    }

    pub fn is_impossible(rstring: Nilable<RString>) -> Result<bool, magnus::Error> {
        Self::is_possible(rstring).map(|b| !b)
    }

    pub fn is_valid_for_country(
        rstring: Nilable<RString>,
        country_code_rstring: Nilable<RString>,
    ) -> Result<bool, magnus::Error> {
        let Nilable::Value(rstring) = rstring else {
            return Ok(false);
        };
        let Nilable::Value(country_code_rstring) = country_code_rstring else {
            return Ok(false);
        };

        let phone_number = unsafe { rstring.as_str() }?;
        let country_code_str = unsafe { country_code_rstring.as_str() }?;
        let country_code_id = phonenumber::country::Id::from_str(country_code_str)
            .map_err(|e| magnus::Error::new(arg_error(), e.to_string()))?;
        let db = &phonenumber::metadata::DATABASE;

        if let Ok(pn) = phonenumber::parse_with(db, Some(country_code_id), phone_number) {
            Ok(pn.country().id() == Some(country_code_id))
        } else {
            Ok(false)
        }
    }

    pub fn is_invalid_for_country(
        rstring: Nilable<RString>,
        country_code_rstring: Nilable<RString>,
    ) -> Result<bool, magnus::Error> {
        Self::is_valid_for_country(rstring, country_code_rstring).map(|b| !b)
    }
}

pub fn init() -> Result<(), magnus::Error> {
    root_module().define_singleton_method("valid?", function!(ClassInterface::is_valid, 1))?;
    root_module().define_singleton_method("parse", function!(ClassInterface::parse, 1))?;
    root_module()
        .define_singleton_method("possible?", function!(ClassInterface::is_possible, 1))?;
    root_module()
        .define_singleton_method("impossible?", function!(ClassInterface::is_impossible, 1))?;
    root_module().define_singleton_method(
        "valid_for_country?",
        function!(ClassInterface::is_valid_for_country, 2),
    )?;
    root_module().define_singleton_method(
        "invalid_for_country?",
        function!(ClassInterface::is_invalid_for_country, 2),
    )?;

    Ok(())
}
