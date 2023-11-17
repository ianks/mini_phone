#[derive(TypedData)]
#[magnus(class = "MiniPhone::GraphQLSchema", free_immediately, size, mark)]
pub struct RubyGraphQLSchema {
    inner_schema: graphql::Schema<graphql::EmptyMutation<graphql::EmptySubscription>>,
}
