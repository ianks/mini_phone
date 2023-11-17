use magnus::{
    value::{InnerValue, Lazy},
    Module, RClass, RModule, Ruby,
};

mod class_interface;
mod phone_number;
mod traits;

pub fn root_module() -> RModule {
    static MODULE: Lazy<RModule> = Lazy::new(|ruby| {
        ruby.define_module("MiniPhone")
            .expect("MiniPhone module not defined")
    });
    let ruby = Ruby::get().expect("Ruby interpreter not initialized");
    MODULE.get_inner_with(&ruby)
}

pub fn phone_number_class() -> RClass {
    static MODULE: Lazy<RClass> = Lazy::new(|_ruby| {
        root_module()
            .define_class("PhoneNumber", magnus::class::object())
            .expect("PhoneNumber module not defined")
    });
    let ruby = Ruby::get().expect("Ruby interpreter not initialized");
    MODULE.get_inner_with(&ruby)
}

#[magnus::init]
pub fn init() -> Result<(), magnus::Error> {
    class_interface::init()?;
    phone_number::init()?;

    Ok(())
}
