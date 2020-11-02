
namespace CGAL {
namespace Surface_mesh_simplification {

/*!
\ingroup PkgSurfaceMeshSimplificationRef

The class `Envelope_filter` is a model for the `GetFilter` concept.
It rejects the placement if the nested filter rejects it, or
if any triangle in the profile is not inside the polyhedral envelope, in this order.


\tparam Get_filter_ must be a model of the concept `GetFilter`.  It defaults to a class that does
not filter any placement.

\cgalModels `GetFilter`

\sa `Envelope`

*/
template <typename GeomTraits, typename GetFilter_>
class Envelope_filter {
public:

  /// The number type
  typedef typename Geom_traits::FT    FT;

  /// \name Creation
  /// @{

  /*!
  %Default constructor
  */
  Envelope_filter();

  /*!
  Constructor

  \param get_filter is the filter that will be filtered.
  */
  Envelope_filter(const FT& dist, const GetFilter_& get_filter);

  /// @}

  /// \name Operations
  /// @{

  /*!
  Returns the placement, if it does not get filtered by the wrapped filter,
  and if all triangles in the profile are inside the polyhedral envelope.
  */
  boost::optional<typename Edge_profile::Point> operator()(const Edge_profile& profile,
                                                           boost::optional<typename Profile::Point> op) const;

  /// @}

}; /* end Surface_mesh_simplification::Envelope_filter */
} // namespace Surface_Mesh_Simplification
} // namespace CGAL
